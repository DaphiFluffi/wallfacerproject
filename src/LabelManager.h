#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include <vector>

class LabelManager
{
private:


    LabelManager() {

    };
public:


    LabelManager(const LabelManager&) = delete;
    LabelManager& operator=(const LabelManager&) = delete;
    static LabelManager& getInstance() {
        static LabelManager instance;
        return instance;
    }

    mousePressed(int x, int y, int button)
};