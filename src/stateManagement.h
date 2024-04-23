#pragma once

#include "ofMain.h"
#include "mediaGrid.h"
#include <vector>
#include <memory>
#include <tuple>
#include <deque>
#include <numeric>
#include <math.h>
#include <optional>

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


enum class ButtonType {
    FULLSCREEN,
    PAUSE_RESUME,
};


struct Button {

    float center_x, center_y;
    float full_width;
    float display_percentage;
    ofColor color = ofColor(0, 255, 0);
    ButtonType type;

    Button(float x, float y, float w, ofColor c, ButtonType tp, float dper =1.0f) : center_x(x), center_y(y),
     full_width(w), color(c), type(tp), display_percentage(dper) {};

    ofRectangle get_rect()
    {
        float width = full_width * display_percentage;
        return ofRectangle(center_x - width / 2, center_y - width / 2, width, width);
    }

    bool is_inside(float x, float y)
    {
        return get_rect().inside(x, y);
    }

    void draw()
    {
        ofSetColor(color);
        ofDrawRectangle(get_rect());
    }


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

        buttons.push_back(Button(0, 0, max_percentage * item_width, ofColor(0, 255, 0), ButtonType::FULLSCREEN, 0.0f));

        if (item->type == MediaType::VIDEO)
        {
            buttons.push_back(Button(0, 0, max_percentage * item_width, ofColor(0, 0, 255), ButtonType::PAUSE_RESUME, 0.0f));
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
        GridState::update();

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
    virtual void draw()
    {
        int screenWidth = ofGetWindowWidth();
        int screenHeight = ofGetWindowHeight();

        float aspectRatio = static_cast<float>(item->start_w) / static_cast<float>(item->start_h);

        float drawWidth, drawHeight;
        if (aspectRatio > 1)
        { 
            drawWidth = screenHeight * aspectRatio;
            drawHeight = screenHeight;
        }
        else
        { 
            drawWidth = screenWidth;
            drawHeight = screenWidth / aspectRatio;
        }

        if (drawWidth > screenWidth)
        {
            drawWidth = screenWidth;
            drawHeight = drawWidth / aspectRatio;
        }
        if (drawHeight > screenHeight)
        {
            drawHeight = screenHeight;
            drawWidth = drawHeight * aspectRatio;
        }

        // Calculate the position to center the mediaItem
        float drawX = (screenWidth - drawWidth) / 2;
        float drawY = (screenHeight - drawHeight) / 2;

        // Draw the mediaItem at the calculated position and size
        item->draw(drawX, drawY, drawWidth, drawHeight);

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

class StateManager
{
private:
    mediaGrid *grid = nullptr;
    std::unique_ptr<State> state = nullptr;

public:
    StateManager(mediaGrid *grid) : grid(grid)
    {
        state = std::make_unique<BrowsingState>(grid);
    };

    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseMoved(int x, int y);
    void mousePressed(int x, int y, int button)
    {
        state->mousePressed(x, y, button);
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
