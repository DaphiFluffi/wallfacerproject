#pragma once

#include "ofMain.h"
#include <memory> 
#include <tuple> 



    

class mediaItem {

private:

public:
    
    int drawingPriotity = 0;
    
    virtual void draw( float display_size = 1.0) = 0;

    virtual void update() = 0;

    virtual void load(const std::string& path) = 0;

    virtual ~mediaItem() = default;


    int start_x, start_y;
    int current_x, current_y, start_w, start_h;

    float display_size ;

    void assign_start_cords(int x, int y, int width, float display_size = 1.0){
        start_x = current_x = x;
        start_y = current_y = y;
        start_w = start_h = width;
        this->display_size = display_size;
    };


    std::tuple<float, float> get_center(){

        return {current_x + start_w / 2, current_y + start_h / 2};

    };

    ofRectangle get_bounding_box(float size){

        auto [center_x, center_y] = get_center();

        float x = center_x - start_w * size / 2;

        float y = center_y - start_h * size / 2;  

        return ofRectangle(x, y, start_w * size, start_h * size);

    }


    ofRectangle get_bounding_box(){

        return get_bounding_box(display_size);
    }

    // BoundingBox get_bounding_box(){

    //     auto [center_x, center_y] = get_center();

    //     float x = center_x - start_w * display_size / 2;

    //     float y = center_y - start_h * display_size / 2;  

    //     return BoundingBox(x, y, x + start_w * display_size, y + start_h * display_size);

    // }

    bool contains(float x, float y){
        auto box = get_bounding_box();
        return box.inside(x, y);
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
        item.draw(box.getLeft(), box.getTop(), start_w * display_size, start_h * display_size);
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
        item.draw(box.getLeft(), box.getTop(), start_w * display_size, start_h * display_size);
    }

    void update() override {};
};

