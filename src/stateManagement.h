#pragma once

#include "ofMain.h"
#include "mediaGrid.h"
#include <vector>
#include <memory> 
#include <tuple> 
#include <deque>
#include <numeric>
#include <math.h>


enum class StateNames {
    NONE,
    DRAGGING,
    BROWSING,
};

class State {

    protected:
        StateNames name = StateNames::NONE;

    public:

    StateNames getName() {return name;};
    State() = default;
    virtual void draw() = 0;
    virtual void update() = 0;
    virtual void mouseMoved(int x, int y) {};
    virtual void mouseDragged(int x, int y, int button) {};
    virtual void mouseReleased(int x, int y, int button) {};
};


class GridState : public State{
    protected:
    mediaGrid* grid = nullptr;


    public:

    GridState(mediaGrid* g) : grid(g) {};

    virtual void draw() override {
        grid->draw();
    }

    virtual void update() {
        grid->update();
    };

};


class RunningMean{
    int steps;
    std::deque<float> values;

    public:
    
        RunningMean(int s): steps(s){};

        float add_val(float v){

            if (values.size() == steps) values.pop_front();

            values.push_back(v);

            return std::accumulate(values.begin(), values.end(), 0.0)/ values.size();

        };
};

class BrowsingState : public GridState{

    std::deque<float> xs;
    std::deque<float> ys;
    int nLast;
    int mouseX, mouseY;

    int n_frames_full = 20;
    int passed_frames = 0;

    float displaySpeedCut = 5.0;

    std::unique_ptr<mediaItem>* active_el = nullptr;


    public:
    BrowsingState(mediaGrid* g, int nLast_steps = 6) : GridState(g), nLast(nLast_steps) {
        name = StateNames::BROWSING;
    };

    void mouseMoved(int x, int y);
    void update();

    float getMouseSpeed(){

        if (!xs.size()) return 0;

        float speed = 0;

        for (int i = 0; i < xs.size() - 1; ++i){
            speed += sqrt(pow(xs[i] - xs[i+1], 2) + pow(ys[i] - ys[i+1], 2));
        }

        return speed / (xs.size() - 1);
    };


};

class DraggingState : public GridState {

    private:
    int start_x, start_y;
    std::unique_ptr<mediaItem>* item = nullptr;

    public:
    DraggingState(int x, int y, mediaGrid* gr, std::unique_ptr<mediaItem>* it) : 
        GridState(gr), start_x(x), start_y(y), item(it) {
            name = StateNames::DRAGGING;
            (*item)->drawingPriotity = 1;
        };

    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
};



class StateManager {
    private:

    mediaGrid* grid = nullptr;
    std::unique_ptr<GridState> state = nullptr;

    public:

    StateManager(mediaGrid* grid) : grid(grid) {
         state = std::make_unique<BrowsingState>(grid);
    };

    void mouseDragged(int x, int y,  int button);
    void mouseReleased(int x, int y, int button);
    void mouseMoved(int x, int y);
    void draw(){ state->draw();};
    void update(){ state->update();};

};

