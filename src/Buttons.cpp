//
// Created by felix on 28.04.24.
//

#include "Buttons.h"

Button get_button(ButtonType type, float width,  float x, float y) {

    switch (type) {
        case ButtonType::FULLSCREEN:
            return Button(x, y, width, ofColor(0, 255, 0), ButtonType::FULLSCREEN);

        case ButtonType::PAUSE_RESUME:
            return Button(x, y, width, ofColor(0, 0, 255), ButtonType::PAUSE_RESUME);

        case ButtonType::SEARCH:
            return Button(x, y, width, ofColor(255, 0, 0), ButtonType::SEARCH);

        default:
            throw std::invalid_argument( "received negative value" );
    }

};

Button get_button(ButtonType type, float width, ofColor color,  float x, float y) {
    Button but = get_button(type, width, x, y);
    but.color = color;
    return but;
};
