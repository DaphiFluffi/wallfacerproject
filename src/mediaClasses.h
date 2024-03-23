//mediaClasses.h
#pragma once

#include "ofMain.h"
#include <vector>
#include <memory> // For std::unique_ptr


struct BoundingBox {
    float left;
    float top;
    float right;
    float bottom;
    
    BoundingBox(float l, float t, float r, float b) 
        : left(l), top(t), right(r), bottom(b) {}

    bool contains(float x, float y){
        return x >= left && x <= right && y >= top && y <= bottom;
    }

};

class mediaItem {
public:
    virtual void draw( float display_size = 1.0) = 0;

    virtual void update() = 0;

    virtual void load(const std::string& path) = 0;

    virtual ~mediaItem() = default;

    int start_x, start_y, start_w, start_h;


    float display_size = 1.0;

    BoundingBox get_bounding_box(){

        int center_x = start_x + start_w / 2;
        float x = center_x - start_w * display_size / 2;

        int center_y = start_y + start_h / 2;
        float y = center_y - start_h * display_size / 2;  

        return BoundingBox(x, y, x + start_w * display_size, y + start_h * display_size);

    }

    bool contains(float x, float y){
        auto box = get_bounding_box();
        return box.contains(x, y);
    }


};



class mediaVideo : public mediaItem {
    ofVideoPlayer item;

    public: 
    void load(const std::string& path) override {
        item.load(path);
        item.setLoopState(OF_LOOP_NORMAL);
	    item.play();
    }

    void update() override {
        item.update();
    };

    void draw( float size = 1.0) override {
        auto box = get_bounding_box();
        item.draw(box.left, box.top, start_w * display_size, start_h * display_size);
    }

};

class mediaImage : public mediaItem {
    ofImage item;

public:
    void load(const std::string& path) override {
        item.load(path);
    }

    void draw( float size = 1.0) override {

        auto box = get_bounding_box();
        item.draw(box.left, box.top, start_w * display_size, start_h * display_size);

    }

    void update() override {};
};




class mediaGrid {
    std::vector<std::vector<std::unique_ptr<mediaItem>>> grid;
    int n_items_x, n_items_y, buffer_between, item_size;

    std::unique_ptr<mediaItem>* active = nullptr;

    float smaller_size = 0.85;
    float larger_size = 1.15;
    int n_frames_full = 20;
    int passed_frames = 0;
    int last_mouse_x, last_mouse_y = 0;

public:
    mediaGrid(int n_items_x, int n_items_y, int buffer_between, int item_size)
        : n_items_x(n_items_x), n_items_y(n_items_y), buffer_between(buffer_between), item_size(item_size) {
        // Initialize the grid with null pointers

        for (int i = 0; i < n_items_x; i++) {

            std::vector<std::unique_ptr<mediaItem>> row;
            for (int j = 0; j < n_items_y; j++) {
                std::unique_ptr<mediaItem> ptr = nullptr;
                row.push_back(std::move(ptr));
            }
            grid.push_back(std::move(row));
        }

    }

    void addItem(std::unique_ptr<mediaItem> item, int x, int y) {
        if (x < n_items_x && y < n_items_y) {

            item->start_x = x * (item_size + buffer_between) + buffer_between;
            item->start_y = y * (item_size + buffer_between) + buffer_between;
            item->start_h = item_size;
            item->start_w = item_size;


            grid[x][y] = std::move(item);


        } else {
            cerr << "Invalid position for item" << endl;
        }

    }

    void draw() {
        for (int y = 0; y < n_items_y; ++y) {
            for (int x = 0; x < n_items_x; ++x) {
                if (grid[x][y]) {
                    grid[x][y]->draw();
                } else {
                    cerr << "Null pointer in grid" << endl;
                }
            }
        }
    }


    void update_display_size(){
        //horible spagtetti code d
        int x = last_mouse_x;
        int y = last_mouse_y;

        bool found = false;

        for (auto& row : grid) {
            for (auto& item : row) {
                if (item && item->contains(x, y)) {
                    
                    // if already active
                    if (active && active == &item){
                        if (passed_frames < n_frames_full) ++passed_frames;

                        item->display_size = (larger_size - 1) * (passed_frames / (float)n_frames_full) + 1;
                        // std::cout << "Display time "<< item->display_size << endl;
                    } else {
                        active = &item;
                        passed_frames = 0;
                    }

                    found = true;


                }
            }
        } 

        if (!found){

            // already the case
            if (!active && passed_frames < n_frames_full){
                passed_frames++;
            } else if (active){
                passed_frames = 0;
            }

            active = nullptr;

        } 
        
        for (auto& row : grid) {
            for (auto& item : row) {
                
                if (!active){

                    if (item-> display_size > 1) {
                        item->display_size = (larger_size - 1) * (1- (passed_frames / (float)n_frames_full)) + 1;
                    }

                    if (item-> display_size < 1) {
                        item->display_size = (smaller_size - 1) * (1- (passed_frames / (float)n_frames_full)) + 1;
                    }

                    

                    item->display_size = (item->display_size);
                }else if (&item == active){
                    // already dealt with on top
                }else{
                    item->display_size = (smaller_size - 1) * (passed_frames / (float)n_frames_full) + 1;

                }
            }
        }

        

    };

    void update(){

        update_display_size();

        for (auto& row : grid) {
            for (auto& item : row) {
                if (item) {
                    item->update();
                }
            }
        }
    }

    void mouseMoved(int x, int y){
    
        last_mouse_x = x;
        last_mouse_y = y;   

    };       

};