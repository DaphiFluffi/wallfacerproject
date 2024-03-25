#pragma once

#include "ofMain.h"
#include "mediaItems.h"
#include <vector>
#include <memory> 
#include <tuple> 
#include <deque>
#include <numeric>
#include <algorithm> 
#include <math.h>


class mediaGrid {
public:
    std::vector<std::vector<std::unique_ptr<mediaItem>>> grid;
    int n_items_x, n_items_y, buffer_between, item_size;


    float smaller_size = 0.85;
    float larger_size = 1.15;


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

            int start_x = x * (item_size + buffer_between) + buffer_between;
            int start_y = y * (item_size + buffer_between) + buffer_between;
            item->assign_start_cords(start_x, start_y, item_size);

            grid[x][y] = std::move(item);


        } else {
            cerr << "Invalid position for item" << endl;
        }

    }

    void draw() {
        // change this such that there is a sorted priority queue depending on grid[x][y]->drawingPriority 
        vector<mediaItem*> items;


        for (int y = 0; y < n_items_y; ++y) {
            for (int x = 0; x < n_items_x; ++x) {
                if (grid[x][y]) {
                    items.push_back(grid[x][y].get());
                } else {
                    cerr << "Null pointer in grid" << endl;
                }
            }
        }

        std::sort(items.begin(), items.end(), [](mediaItem* a, mediaItem* b) {
            return a->drawingPriotity < b->drawingPriotity;
        });

        for (auto item : items) {
            item->draw();
        };

    }

    unique_ptr<mediaItem>* get_item_by_cords(int x, int y){

        for (auto& row : grid) {
                for (auto& item : row) {
                    if (item && item->contains(x, y)) return &item;
                }    
        }
        return nullptr;
    }

    vector<unique_ptr<mediaItem>*> get_items_by_cords(int x, int y){
        vector<unique_ptr<mediaItem>*> items;

       for (auto& row : grid) {
                for (auto& item : row) {
                    if (item && item->contains(x, y)) items.push_back(&item);
                }    
        }
        return items;
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
};