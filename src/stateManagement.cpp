
#include "stateManagement.h"


void StateManager::mouseMoved(int x, int y){
    state->mouseMoved(x, y);
}



void StateManager::mouseDragged(int x, int y, int button){

    if (button != 0) return;

    auto name = state->getName();

    if (name == StateNames::BROWSING || name == StateNames::ITEM_MENU){


        auto item = current_grid->get_item_by_cords(x, y);
        
        if (!item || item->is_empty()) return;


        state = std::make_unique<DraggingState>(x, y, current_grid, item);
    }
    
    state->mouseDragged(x, y, 0);
}

void StateManager::mouseReleased(int x, int y, int button){
    state->mouseReleased(x,y,button);
};

void DraggingState::mouseReleased(int x, int y, int button){

    if (button != 0) return;
    finished = true;
    auto [center_x, center_y] = item->get_center();

    item->drawingPriotity = 0;

    if (!grid->inside(center_x, center_y)) {
        std::cout << "outside grid -> dropping item" << endl;
        auto popped = grid->pop_item(item);
        if (!popped) {
            std::cerr << "Could not pop item" << std::endl;
            return;
        }

        StateManager::getInstance().addToGridAbove(std::move(popped));


        return ;
    }

    auto matchedItems = grid->get_items_by_cords(center_x, center_y);



    item->drawingPriotity = 0;
    item->current_x = item->start_x;
    item->current_y = item->start_y;

    mediaItem* other_item = nullptr;
    
    for (auto other : matchedItems){
        if (item != other){
            other_item = other;
        }
    }

    if( !matchedItems.size()){
        std::cout << "matched with nothing" << endl;
    } else if (!other_item) {
        std::cout << "only found highlighted" << endl;

        return;


    }

    std::cout << "other item type id " << static_cast<int>(other_item->type) << endl;

        // if collector move and put in collector
    if (other_item->type == MediaType::COLLECTOR){

        std::cout << "collector swap try" << endl;

        collectorItem* collector = dynamic_cast<collectorItem*>(other_item);
        if (!collector){
            cerr << "Could not cast to collector" << endl;
            return;
        }

        auto [x,y] = grid->get_cords_by_item(item);
        if (x<0 || y< 0) {
            std::cerr << "Somehow negative coords" << std::endl;
            return;
        }

        auto selected_item = grid->pop_item(x, y);

        if (!selected_item) {std::cerr << "could not pop item" << std::endl; return;}
        collector->add_item(std::move(selected_item));

    } else {



        cout << "trying to swap" << endl;

        std::swap(item->start_x, other_item->start_x);
        std::swap(item->start_y, other_item->start_y);
        item->current_x = item->start_x;
        item->current_y = item->start_y;
        other_item->current_x = other_item->start_x;
        other_item->current_y = other_item->start_y;



        std::swap(item, other_item);
    } 

    
}

bool StateManager::keyPressed(int key) {
    switch (key) {
        case 'u':
            std::cout << "Moving out of Window" << std::endl;
            popWindow();
            return true;
        default:
            return false;
    }
}



unique_ptr<State> DraggingState::move_to_new_state(){

    if(finished)
        return std::make_unique<BrowsingState>(grid);

    return nullptr;
};



void BrowsingState::update() {
    mouseX = ofGetMouseX();
    mouseY = ofGetMouseY();

    
    GridState::update();

    // update last possitions
    if (xs.size() == nLast){
        xs.pop_front();
        ys.pop_front();
    }

    xs.push_back(mouseX);
    ys.push_back(mouseY);

    if (getMouseSpeed() > displaySpeedCut) return;

    mediaItem* item = grid->get_item_by_cords(mouseX, mouseY);

    // adjust frame counts
    if (item && !item->is_empty()){  // if already active_el
        if (active_el && active_el == item){
            if (passed_frames < n_frames_full) ++passed_frames;

        } else {
            active_el = item;
            passed_frames = 0;
        }

    } else {
        // already the case
        if (!active_el && passed_frames < n_frames_full){
            passed_frames++;
        } else if (active_el)  //freshly set to 0
            passed_frames = 0;

        active_el = nullptr;   
    }

    //adjust sizes of blocks
    for (auto& row : grid->grid) {
        for (auto& it : row) {
            
            // revert back to normal size
            if (!active_el){

                if (it-> display_size > 1) {
                    it->display_size = (grid->larger_size - 1) * (1- (passed_frames / static_cast<float>(n_frames_full))) + 1;
                }

                if (it-> display_size < 1) {
                    it->display_size = (grid->smaller_size - 1) * (1- (passed_frames / static_cast<float>(n_frames_full))) + 1;
                }


                it->display_size = (it->display_size);
            }else if (it.get() == active_el){
                it->display_size = (grid->larger_size - 1) * (passed_frames / static_cast<float>(n_frames_full)) + 1;

            }else{
                it->display_size = (grid->smaller_size - 1) * (passed_frames / static_cast<float>(n_frames_full)) + 1;

            }
        }
    }
};

unique_ptr<State> BrowsingState::move_to_new_state(){
    if(active_el && passed_frames == n_frames_full){

        return std::make_unique<ItemMenuState>(grid, active_el);
    }

    return nullptr;

}

void DraggingState::mouseDragged(int x, int y, int button){

        // std::cout << "dragged" << x <<" "<< y <<" "<< (*item)->current_x << " " << (*item)->current_y << std::endl; 

        int x_diff = x - start_x;
        int y_diff = y - start_y;
        item->current_x = item->start_x + x_diff;
        item->current_y = item->start_y +y_diff;

};


    

void ItemMenuState::mousePressed(int x, int y, int button)
{

    auto type = get_button_type(x, y);

    if (type.has_value())
    {
        switch (type.value())
        {
        case ButtonType::FULLSCREEN:

            if (item->type == MediaType::COLLECTOR) {

                collectorItem* collector = dynamic_cast<collectorItem*>(item);
                if (!collector){
                    cerr << "Could not cast to collector in fullscreen move" << endl;
                    return;
                }

                StateManager::getInstance().addWindow(std::make_unique<DisplayWindow>(collector->getGrid()));
            }else {

                new_state = std::make_unique<FullScreenMode>(item, grid);
            }


            break;
        case ButtonType::PAUSE_RESUME:
            item->togglePause();
            break;
        }
    }




};

unique_ptr<State> ItemMenuState::move_to_new_state(){
    
    if(new_state)
        return std::move(new_state);

    if(!item->contains(ofGetMouseX(), ofGetMouseY()))
        return std::make_unique<BrowsingState>(grid);

    return nullptr;
};
