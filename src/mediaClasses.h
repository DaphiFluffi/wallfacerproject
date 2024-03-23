//mediaClasses.h
#pragma once

#include "ofMain.h"
#include <vector>
#include <memory> // For std::unique_ptr


class mediaItem {
public:
    virtual void draw(int x, int y, int width, int height) = 0;

    virtual void update() = 0;

    virtual void load(const std::string& path) = 0;

    virtual ~mediaItem() = default;

    int start_x, start_y, start_w, start_h;

    float displa_size = 1.0;
};


class mediaImage : public mediaItem {
    ofImage item;

public:
    void load(const std::string& path) override {
        item.load(path);
    }

    void draw(int x, int y, int width, int height) override {
        item.draw(x, y, width, height);
        cout << "Drawing image at " << x << ", " << y << endl;
    }

    void update() override {};
};




class mediaGrid {
    std::vector<std::vector<std::unique_ptr<mediaItem>>> grid;
    int n_items_x, n_items_y, buffer_between, item_size;

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
                    grid[x][y]->draw(posX, posY, item_size, item_size);
                } else {
                    cerr << "Null pointer in grid" << endl;
                }
            }
        }
    }

    void update(){
        for (auto& row : grid) {
            for (auto& item : row) {
                if (item) {
                    item->update();
                }
            }
        }
    }

    void mouseEntered(int x, int y){
    };

};