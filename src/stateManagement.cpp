
#include "stateManagement.h"


void StateManager::mouseMoved(int x, int y){
    state->mouseMoved(x, y);
}



void StateManager::mouseDragged(int x, int y, int button){

    if (button != 0) return;
    if (state->getName() != StateNames::DRAGGING ){


        auto item = grid->get_item_by_cords(x, y);    
        
        if (!item) return;


        state = std::make_unique<DraggingState>(x, y, grid, item);
    }
    
    state->mouseDragged(x, y, 0);
}

void StateManager::mouseReleased(int x, int y, int button){
  
        std::cout << "Mousereleased" << x << " " << y << " " << button  << std::endl; 

        if (button != 0) return;

        if (state->getName() == StateNames::DRAGGING){

            state->mouseReleased(x, y, button);


            state = make_unique<BrowsingState>(grid);
        }
};

void DraggingState::mouseReleased(int x, int y, int button){

    if (button != 0) return;

    auto [center_x, center_y] = (*item)->get_center();

    auto matchedItems = grid->get_items_by_cords(center_x, center_y);



    (*item)->drawingPriotity = 0;
    (*item)->current_x = (*item)->start_x;
    (*item)->current_y = (*item)->start_y;

    unique_ptr<mediaItem>* other_item = nullptr;
    
    for (auto other : matchedItems){
        if (item != other){
            other_item = other;
        }
    }

    if( !matchedItems.size()){
        std::cout << "matched with nothing" << endl;
    } else if (!other_item) {
        std::cout << "only found highlighted" << endl;

    } else {



        cout << "trying to swap" << endl;

        std::swap((*item)->start_x, (*other_item)->start_x);
        std::swap((*item)->start_y, (*other_item)->start_y);
        (*item)->current_x = (*item)->start_x;
        (*item)->current_y = (*item)->start_y;
        (*other_item)->current_x = (*other_item)->start_x;
        (*other_item)->current_y = (*other_item)->start_y;



        std::swap((*item), (*other_item));
        return;
    } 

    

}

void BrowsingState::mouseMoved(int x, int y){
    // mouseX = x; mouseY = y;


}

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

    auto item = grid->get_item_by_cords(mouseX, mouseY);

    // adjust frame counts
    if (item){  // if already active_el
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
        for (auto& item : row) {
            
            // revert back to normal size
            if (!active_el){

                if (item-> display_size > 1) {
                    item->display_size = (grid->larger_size - 1) * (1- (passed_frames / (float)n_frames_full)) + 1;
                }

                if (item-> display_size < 1) {
                    item->display_size = (grid->smaller_size - 1) * (1- (passed_frames / (float)n_frames_full)) + 1;
                }


                item->display_size = (item->display_size);
            }else if (&item == active_el){
                item->display_size = (grid->larger_size - 1) * (passed_frames / (float)n_frames_full) + 1;

            }else{
                item->display_size = (grid->smaller_size - 1) * (passed_frames / (float)n_frames_full) + 1;

            }
        }
    }
};


void DraggingState::mouseDragged(int x, int y, int button){

        // std::cout << "dragged" << x <<" "<< y <<" "<< (*item)->current_x << " " << (*item)->current_y << std::endl; 

        int x_diff = x - start_x;
        int y_diff = y - start_y;
        (*item)->current_x = (*item)->start_x + x_diff;
        (*item)->current_y = (*item)->start_y +y_diff;

};