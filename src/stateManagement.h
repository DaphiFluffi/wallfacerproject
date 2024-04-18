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
        ITEM_MENU,
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
        virtual unique_ptr<State> move_to_new_state() {return nullptr;};

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



    class BrowsingState : public GridState{

        std::deque<float> xs;
        std::deque<float> ys;
        int nLast;
        int mouseX, mouseY;

        int n_frames_full = 20;
        int passed_frames = 0;

        float displaySpeedCut = 5.0;

        mediaItem* active_el = nullptr;


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

        virtual unique_ptr<State> move_to_new_state();


    };

    class DraggingState : public GridState {

        private:
        int start_x, start_y;
        mediaItem* item = nullptr;

        public:
        DraggingState(int x, int y, mediaGrid* gr, mediaItem* it) : 
            GridState(gr), start_x(x), start_y(y), item(it) {
                name = StateNames::DRAGGING;
                item->drawingPriotity = 1;
            };

        void mouseDragged(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
    };


    class ItemMenuState : public GridState {

        private:
        mediaItem* item = nullptr;
        int steps;
        float max_percentage = 0.2;
        float full_steps = 10;

        public:
        ItemMenuState(mediaGrid* gr, mediaItem* it) : 
            GridState(gr), item(it) {
                name = StateNames::ITEM_MENU;
                steps = 0;
            };

        void mouseDragged(int x, int y, int button);
        void mouseReleased(int x, int y, int button);

        virtual void update(){
            steps++;
            GridState::update();

        }

        ofRectangle get_menu_rect(){
            float size = std::min(steps / full_steps, 1.0f) * max_percentage;

            return item->get_bounding_box(size);
        };

        virtual void draw(){
            GridState::update();

            
            ofDrawRectangle(get_menu_rect());
        }


    };


    class StateManager {
        private:

        mediaGrid* grid = nullptr;
        std::unique_ptr<State> state = nullptr;

        public:

        StateManager(mediaGrid* grid) : grid(grid) {
            state = std::make_unique<BrowsingState>(grid);
        };

        void mouseDragged(int x, int y,  int button);
        void mouseReleased(int x, int y, int button);
        void mouseMoved(int x, int y);
        void draw(){ state->draw();};
        void update(){ 
            
            state->update();
            
            auto new_state = state->move_to_new_state();

            if (new_state) state = std::move(new_state);
            
            };

    };
