//
// Created by felix on 28.04.24.
//
#pragma once
#include "ofMain.h"


enum class ButtonType {
    FULLSCREEN,
    PAUSE_RESUME,
    SEARCH,
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

Button get_button(ButtonType type, float width,  float x = 0, float y = 0);

Button get_button(ButtonType type, float width, ofColor color,  float x = 0, float y = 0);