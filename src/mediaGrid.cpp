
#include "mediaGrid.h"


mediaGrid::mediaGrid(int n_items_x, int n_items_y, float buffer_between, float item_size)
    : n_items_x(n_items_x), n_items_y(n_items_y), buffer_between(buffer_between), item_size(item_size) {
    // Initialize the grid with null pointers

    for (int i = 0; i < n_items_x; i++) {
        std::vector<std::unique_ptr<mediaItem> > row;
        for (int j = 0; j < n_items_y; j++) {
            std::unique_ptr<mediaItem> ptr = std::make_unique<emptyItem>();
            row.push_back(std::move(ptr));
        }
        grid.push_back(std::move(row));
    }
}

void mediaGrid::assignTo(mediaItem* item, int x, int y) {

        float magic_constant = (x_start || y_start) ? 0.5f: 1.0f;

        int start_x = x * (item_size + buffer_between) + buffer_between + x_start;
        int start_y = y * (item_size + buffer_between) + buffer_between * magic_constant + y_start;

        item->assign_start_cords(start_x, start_y, item_size);
}

void mediaGrid::addItem(std::unique_ptr<mediaItem> item, int x, int y) {


    if(!item){
        cerr << "Tried to add nullptr to grid" << endl;
        return;
    }

    if (x < n_items_x && y < n_items_y) {
        assignTo(item.get(), x, y);
        grid[x][y] = std::move(item);
    } else {
        cerr << "Invalid position for item" << endl;
    }
}

std::optional<std::tuple<int, int>> mediaGrid::first_free_cords() {


    for (int j = 0; j < n_items_y; j++) {
        for (int i = 0; i < n_items_x; i++) {
            if (!grid[i][j] || grid[i][j]->type == MediaType::EMPTY) {
                return std::make_tuple(i, j);
            }
        }
    }
    return std::nullopt;

};

vector<mediaItem *> mediaGrid::get_n_first_full(int n, bool skip_collector) {

    vector<mediaItem *> vec = {};

    if (n <= 0) return vec;

    for (int i = 0; i < n_items_x; i++) {
        for (int j = 0; j < n_items_y; j++) {
            if (grid[i][j] && !grid[i][j]->is_empty() && (!skip_collector || grid[i][j]->type != MediaType::COLLECTOR)) {
                vec.push_back(grid[i][j].get());
                if (vec.size() == n) return vec;
            }
        }
    }

    return vec;
};





void mediaGrid::addItem(std::unique_ptr<mediaItem> item) {

    if(!item){
        cerr << "Tried to add nullptr to grid" << endl;
        return;
    }

    auto cords = first_free_cords();

    if (cords) {
        auto [x,y] = cords.value();


        assignTo(item.get(), x, y);
        grid[x][y] = std::move(item);

    } else {
        std::cout << "grif full moving to buffer" << std::endl;
    }


}


void mediaGrid::draw() {
    // change this such that there is a sorted priority queue depending on grid[x][y]->drawingPriority
    vector<mediaItem *> items;


    for (int y = 0; y < n_items_y; ++y) {
        for (int x = 0; x < n_items_x; ++x) {
            if (grid[x][y]) {
                items.push_back(grid[x][y].get());
            } else {
                cerr << "Null pointer in grid" << endl;
            }
        }
    }

    std::sort(items.begin(), items.end(), [](mediaItem *a, mediaItem *b) {
        return a->drawingPriotity < b->drawingPriotity;
    });

    for (auto item: items) {
        item->draw();
    };
}

mediaItem* mediaGrid::get_item_by_cords(int x, int y) {
    for (auto &row: grid) {
        for (auto &item: row) {
            if (item && item->contains(x, y)) return item.get();
        }
    }
    return nullptr;
}

vector<mediaItem *> mediaGrid::get_items_by_cords(int x, int y) {
    vector<mediaItem *> items;

    for (auto &row: grid) {
        for (auto &item: row) {
            if (item && item->contains(x, y)) items.push_back(item.get());
        }
    }
    return items;
}

std::unique_ptr<mediaItem> mediaGrid::pop_item(int x, int y) {
    if (x < n_items_x && y < n_items_y) {
        auto item = std::move(grid[x][y]);

        addItem(std::make_unique<emptyItem>(), x, y);


        return std::move(item);
    } else {
        cerr << "Invalid position for item" << endl;
        return nullptr;
    }
}

std::unique_ptr<mediaItem> mediaGrid::pop_item(mediaItem *item) {
    for (int i = 0; i < n_items_x; i++) {
        for (int j = 0; j < n_items_y; j++) {
            if (grid[i][j].get() == item) {
                return pop_item(i, j);
            }
        }
    }
    return nullptr;
}

tuple<int, int> mediaGrid::get_cords_by_item(mediaItem *item) {
    for (int i = 0; i < n_items_x; i++) {
        for (int j = 0; j < n_items_y; j++) {
            if (grid[i][j].get() == item) {
                return make_tuple(i, j);
            }
        }
    }
    return make_tuple(-1, -1);
}


void mediaGrid::update() {
    for (auto &row: grid) {
        for (auto &item: row) {
            if (item) {
                item->update();
            }
        }
    }
}

int mediaGrid::size() {
    int i = 0;
    for (int i = 0; i < n_items_x; i++) {
        for (int j = 0; j < n_items_y; j++) {
            if (grid[i][j] && !grid[i][j]->is_empty()) ++i;
        }
    }

    return i;
}

void mediaGrid::clear() {
    for (int i = 0; i < n_items_x; i++) {
        for (int j = 0; j < n_items_y; j++) {
            if (grid[i][j]->type != MediaType::COLLECTOR)
                grid[i][j] = std::make_unique<emptyItem>();

        }
    }
};


void mediaGrid::adjustToCenteredSquare(float buffer_percentage) {

    auto [drawX, drawY, drawWidth, drawHeight] = get_centerted_box();

    x_start = drawX;
    y_start = drawY;
    width = drawWidth;
    height = drawHeight;

    buffer_between = drawWidth * buffer_percentage;
    item_size = (drawWidth - buffer_between) / n_items_x - buffer_between;

    for (int i = 0; i < n_items_x; i++) {
        for (int j = 0; j < n_items_y; j++) {
            if (grid[i][j])
                assignTo(grid[i][j].get(), i, j);
        }
    }
}

ofRectangle mediaGrid::get_bounding_box() {
    return ofRectangle(x_start, y_start, width, height);
}

bool mediaGrid::inside(float x, float y) {
    return get_bounding_box().inside(x, y);
}
