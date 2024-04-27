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
#include <optional>
#include "utils.h"


class mediaGrid {
public:
    std::vector<std::vector<std::unique_ptr<mediaItem> > > grid;
    std::vector<std::unique_ptr<mediaItem> > buffer;


    int n_items_x, n_items_y, buffer_between, item_size;
    int x_start = 0;
    int y_start = 0;
    float width = ofGetWindowWidth();
    float height = ofGetWindowHeight();

    float smaller_size = 0.85;
    float larger_size = 1.15;


    mediaGrid(int n_items_x, int n_items_y, float buffer_between, float item_size);

    void adjustToCenteredSquare(float);

    void assignTo(mediaItem* item, int x, int y);

    void addItem(std::unique_ptr<mediaItem> item, int x, int y);
    void addItem(std::unique_ptr<mediaItem> item);

    void draw();

    mediaItem *get_item_by_cords(int x, int y);

    int size();

    vector<mediaItem *> get_items_by_cords(int x, int y);
    vector<mediaItem *> get_n_first_full(int n, bool skip_collector = true);

    std::unique_ptr<mediaItem> pop_item(int x, int y);

    std::unique_ptr<mediaItem> pop_item(mediaItem *item);

    std::optional<std::tuple<int, int>> first_free_cords();

    tuple<int, int> get_cords_by_item(mediaItem *item);

    void update();

    ofRectangle get_bounding_box();
    bool inside(float x, float y);

};
