//
// Created by felix on 03.06.24.
//

#include "MotionDetector.h"


ofRectangle translate(ofRectangle rectangle, float x, float y) {
    return ofRectangle(rectangle.x + x, rectangle.y + y, rectangle.width, rectangle.height);
}

ofRectangle scaleFromCenter(ofRectangle rect, float factor) {
    auto center = rect.getCenter();

    float cw = factor * rect.width;
    float ch =  factor * rect.height;
    return ofRectangle(center.x - cw / 2, center.y - ch/2, cw, ch);
}