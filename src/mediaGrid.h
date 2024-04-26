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
    std::vector<std::vector<std::unique_ptr<mediaItem> > > grid;
    int n_items_x, n_items_y, buffer_between, item_size;


    float smaller_size = 0.85;
    float larger_size = 1.15;


    mediaGrid(int n_items_x, int n_items_y, int buffer_between, int item_size);

    void addItem(std::unique_ptr<mediaItem> item, int x, int y);

    void draw();

    mediaItem *get_item_by_cords(int x, int y);

    vector<mediaItem *> get_items_by_cords(int x, int y);

    std::unique_ptr<mediaItem> pop_item(int x, int y);

    std::unique_ptr<mediaItem> pop_item(mediaItem *item);

    tuple<int, int> get_cords_by_item(mediaItem *item);

    void update();
};
