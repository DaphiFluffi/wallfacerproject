#pragma once

#include "ofMain.h"
#include "mediaGrid.h"
#include "higherItems.h"
#include <vector>
#include <deque>
#include <memory>
#include <tuple>
#include <numeric>
#include <math.h>
#include <optional>
#include "utils.h"
#include "ofxGui.h"
#include "searchManager.h"

enum class StateNames
{
    NONE,
    DRAGGING,
    BROWSING,
    ITEM_MENU,
    FULL_SCREEN,
};

class State
{

protected:
    StateNames name = StateNames::NONE;

public:
    StateNames getName() { return name; };
    State() = default;
    virtual  ~State() = default;
    virtual void draw() = 0;
    virtual void update() = 0;
    virtual void mouseMoved(int x, int y){};
    virtual void mousePressed(int x, int y, int button){};
    virtual void mouseDragged(int x, int y, int button){};
    virtual void mouseReleased(int x, int y, int button){};

    virtual unique_ptr<State> move_to_new_state() { return nullptr; };
};

class GridState : public State
{
protected:
    mediaGrid *grid = nullptr;

public:
    GridState(mediaGrid *g) : grid(g){};

    virtual void draw() override
    {
        grid->draw();
    }

    virtual void update()
    {
        grid->update();
    };
};

class BrowsingState : public GridState
{

    std::deque<float> xs;
    std::deque<float> ys;
    int nLast;
    int mouseX, mouseY;

    int n_frames_full = 20;
    int passed_frames = 0;

    float displaySpeedCut = 5.0;

    mediaItem *active_el = nullptr;

public:
    BrowsingState(mediaGrid *g, int nLast_steps = 6) : GridState(g), nLast(nLast_steps)
    {
        name = StateNames::BROWSING;
    };

    void update();

    float getMouseSpeed()
    {

        if (!xs.size())
            return 0;

        float speed = 0;

        for (int i = 0; i < xs.size() - 1; ++i)
        {
            speed += sqrt(pow(xs[i] - xs[i + 1], 2) + pow(ys[i] - ys[i + 1], 2));
        }

        return speed / (xs.size() - 1);
    };

    virtual unique_ptr<State> move_to_new_state();
};

class DraggingState : public GridState
{

private:
    int start_x, start_y;
    mediaItem *item = nullptr;
    bool finished = false;

public:
    DraggingState(int x, int y, mediaGrid *gr, mediaItem *it) : GridState(gr), start_x(x), start_y(y), item(it)
    {
        name = StateNames::DRAGGING;
        item->drawingPriotity = 1;
    };

    virtual void mouseDragged(int x, int y, int button);
    virtual void mouseReleased(int x, int y, int button);

    virtual unique_ptr<State> move_to_new_state();
};





class ButtonManagerState : public GridState {
protected:
    std::vector<Button> buttons;

public:
    ButtonManagerState(mediaGrid *gr) : GridState(gr) {}

    virtual void drawButtons() {
        for (auto &button : buttons) {
            button.draw();
        }
    }

    virtual std::optional<ButtonType> get_button_type(int x, int y) {
        for (auto &button : buttons) {
            if (button.is_inside(x, y))
                return button.type;
        }
        return std::nullopt;
    }

    void possitionButtons(float center_x,float center_y, float radius){

        if (buttons.size() ==0){
            return;
        }
        else if(buttons.size() ==1){
            buttons[0].center_x = center_x;
            buttons[0].center_y = center_y;

        } 
            // in circle
            else 
        {
            float angle_increment = 360.0f / buttons.size();

            for (int i = 0; i < buttons.size(); i++)
            {
                float angle = angle_increment * i;
                float rad = angle * DEG_TO_RAD; 
                float x = center_x + radius * cos(rad);
                float y = center_y + radius * sin(rad);

                
                buttons[i].center_x = x;
                buttons[i].center_y = y;
            };
        };
    };

};


class ItemMenuState : public ButtonManagerState
{

private:
    mediaItem *item = nullptr;
    int steps;
    float max_percentage = 0.15;
    float full_steps = 10;

    std::unique_ptr<State> new_state = nullptr;

public:
    ItemMenuState(mediaGrid *gr, mediaItem *it) : ButtonManagerState(gr), item(it)
    {
        name = StateNames::ITEM_MENU;
        steps = 0;

        float item_width = item->display_size * item->start_w;

        buttons.push_back(get_button(ButtonType::FULLSCREEN, max_percentage * item_width));

        if (item->type == MediaType::IMAGE || item->type == MediaType::VIDEO) {
            buttons.push_back(get_button(ButtonType::SEARCH, max_percentage * item_width));

        }

        if (item->type == MediaType::VIDEO)
        {
            buttons.push_back(get_button(ButtonType::PAUSE_RESUME, max_percentage * item_width));
        }

        auto [center_x, center_y] = item->get_center();

        possitionButtons(center_x, center_y, item_width * 0.2);

    };



    virtual void update()
    {
        steps++;

        for (auto &button : buttons)
        {
            button.display_percentage = std::min(steps / full_steps, 1.0f);
        }

        GridState::update();
    }

    virtual void mousePressed(int x, int y, int button);

    virtual void draw()
    {

        grid->draw();
        
        drawButtons();
    }

    virtual unique_ptr<State> move_to_new_state();
};

class FullScreenMode : public ButtonManagerState
{

private:
    mediaItem *item = nullptr;
    bool back_to_browsing = false;


public:
    FullScreenMode(mediaItem *pointer, mediaGrid *gr) : ButtonManagerState(gr),item(pointer)
    {
        name = StateNames::FULL_SCREEN;

        if (item->type == MediaType::VIDEO)
        {
            buttons.push_back(Button(0, 0, 60, ofColor(0, 0, 255), ButtonType::PAUSE_RESUME, 1));
        }

    };
    virtual void update()
    {
        item->update();
    };
    virtual void draw() // later: more efficient
    {


        auto [drawX, drawY, drawWidth, drawHeight] = get_centerted_box(static_cast<float>(item->start_w)/item->start_h);

        // Draw the mediaItem at the calculated position and size
        item->draw(drawX, drawY, drawWidth, drawHeight);

        float screenWidth = ofGetScreenWidth();
        float screenHeight = ofGetScreenHeight();

        float meta_x = drawX + drawWidth + 30;
        float meta_w = screenWidth - meta_x - 30;
        float meta_y = 150;
        float meta_h = screenHeight - 2 * meta_y;

        item->drawMetadata(meta_x, meta_y, meta_w, meta_h);


        possitionButtons(drawX / 2, screenHeight / 2, 100);
        drawButtons();


    };

    void mousePressed(int x, int y, int button)
    {

        auto button_type = get_button_type(x, y);

        if (button_type)
        {
            switch (button_type.value())
            {
            case ButtonType::PAUSE_RESUME:
                item->togglePause();
                return;
            
            default:
                break;
            }
        }

        back_to_browsing = true;
    }

    virtual unique_ptr<State> move_to_new_state()
    {
        if (back_to_browsing)
        {
            return std::make_unique<BrowsingState>(grid);
        }
        return nullptr;
    }

};



class searchGrid : public mediaGrid {

    mediaItem* item;
    float buffer_percentage = 0.05f;

    ofxPanel panel;
    ofParameterGroup mode_settings;
    vector<ofParameter<bool>> modes = vector<ofParameter<bool>>(2);

    ofParameterGroup sim_functions;
    vector<ofParameter<bool>> methods = vector<ofParameter<bool>>(3);


    ofParameter<bool> start_search;

    searchModes active_mode = searchModes::BRIGHTNESS;
    distanceMetrics active_metric = get_default_metric(active_mode);

    SearchManager& search_manager = SearchManager::getInstance();

public:

    searchGrid(mediaItem* it): item(it),
        mediaGrid(3, 3, 30, 300) {

        mode_settings.setName("Search By");
        mode_settings.add(modes[0].set("Brightness", true));
        mode_settings.add(modes[1].set("Color", false));

        sim_functions.setName("Method");
        sim_functions.add(methods[0].set("MAE", false));
        sim_functions.add(methods[1].set("MSE", false));
        sim_functions.add(methods[2].set("Cosine Sim", false));


        panel.setup("Similarity Search Settings");
        panel.add(mode_settings);
        panel.add(sim_functions);
        panel.add(start_search.set("Search", false));


        adjustToCenteredSquare(buffer_percentage);


        float left_middle = static_cast<float>(x_start)/2;

        panel.setPosition(left_middle - panel.getWidth()/2,100);

        auto box = item->get_bounding_box(larger_size);
        item->current_x = left_middle - box.width/2;
        item->current_y = 400;

    };


    ~searchGrid() {
        panel.unregisterMouseEvents();
        panel.clear();

        item->reset();
    };

    void draw() {
        mediaGrid::draw();

        panel.draw();
        item->draw(larger_size);
    };

    void search_and_fill() {
        auto imgs = search_manager.search_imgs(item, active_mode, active_metric, 4);

        std::cout << "First Image" << imgs[0].data.filePath << std::endl;

        clear();
        for (auto& img : imgs) {

            std::cout << "Adding Image" << img.data.filePath << std::endl;

            auto media_item = std::make_unique<mediaImage>(img.data);
            //media_item->score = img.score;
            addItem(std::move(media_item));

        }

    };



    void update()
    {

        bool serchModeChanged = false;

        mediaGrid::update();
        for (size_t i = 0; i < modes.size(); i++)
        {
            if (modes[i] && i!= static_cast<int>(active_mode))
            {
                active_mode = static_cast<searchModes>(i);
                serchModeChanged = true;
                break;
            }
        }

        for (size_t i = 0; i < modes.size(); i++)
        {
            modes[i].set(i == static_cast<int>(active_mode));
        };

        if (serchModeChanged) {
            active_metric = get_default_metric(active_mode);

        } else {
            for (size_t i = 0; i < methods.size(); i++)
            {
                if (methods[i] && i+1!= static_cast<int>(active_metric) && !invalidMetricCombination(active_mode, distanceMetrics(i+1)))
                {
                    active_metric = static_cast<distanceMetrics>(i +1);

                    break;
                }
            }


        }
        for (size_t i = 0; i < methods.size(); i++)
        {
            methods[i].set(i+1 == static_cast<int>(active_metric));

            std::cout << "i " << 0 << " " << methods[i] << "mode "  << (int) active_mode << "metric " << (int) active_metric << endl;

        };

        if (start_search)
        {
            start_search.set(false);
            search_and_fill();
        }


    }

    mediaItem *get_item_by_cords(int x, int y){
        return mediaGrid::get_item_by_cords(x,y);
    };
    vector<mediaItem *> get_items_by_cords(int x, int y){
        return mediaGrid::get_items_by_cords(x,y);
    };


};




struct DisplayWindow {
    int level;
    mediaGrid* grid;

    DisplayWindow(mediaGrid* g) : grid(g) {};
    virtual ~DisplayWindow() = default;

    virtual std::unique_ptr<State> getDefaultState() {
        return std::move(std::make_unique<BrowsingState>(grid));
    };

    mediaGrid* getGrid() const {
        return grid;
    }
};

struct SearchWindow : public DisplayWindow {

    searchGrid search_grid;

    SearchWindow(mediaItem* it): search_grid(it), DisplayWindow(&search_grid){
        if (!it) {
            std::cerr << "Item to Search Window is None" << std::endl;
        }

    }

};


class StateManager
{
private:

    std::deque<std::unique_ptr<DisplayWindow>> windows;
    mediaGrid *current_grid = nullptr;
    std::unique_ptr<State> state = nullptr;

    StateManager() {

    };
public:


    StateManager(const StateManager&) = delete;
    StateManager& operator=(const StateManager&) = delete;
    static StateManager& getInstance() {
        static StateManager instance;
        return instance;
    }

    void addWindow(std::unique_ptr<DisplayWindow> window) {
        window->level = static_cast<int>(windows.size());
        windows.push_back(std::move(window));

        setToBack();
    };

    void popWindow() {
        if (windows.size() == 1) {
            std::cerr << "Cannot pop the last window" << std::endl;
            return;
        };
        // revert to standard
        windows.back()->getDefaultState()->update();
        windows.pop_back();
        setToBack();
    };

    void setToBack() {
        current_grid = windows.back()->getGrid();
        state = std::make_unique<BrowsingState>(current_grid);
    }

    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseMoved(int x, int y);
    void mousePressed(int x, int y, int button)
    {
        state->mousePressed(x, y, button);
    }

    bool keyPressed(int key);

    void addToGridAbove(std::unique_ptr<mediaItem> item) {

        if (windows.size() >= 2) {
            auto second_last_grid = (*(windows.end() - 2))->getGrid(); 
            second_last_grid->addItem(std::move(item));
        }
        else {
            std::cout << "Cannot add to grid above, there is no grid above" << std::endl;
        }
    }

    void draw() { state->draw(); };
    void update()
    {

        state->update();

        auto new_state = state->move_to_new_state();

        if (new_state)
            state = std::move(new_state);
    };
};
