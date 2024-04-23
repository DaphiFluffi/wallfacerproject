// fontManager.h
#pragma once

#include "ofMain.h"

class fontManager {
private:
    ofTrueTypeFont regularFont;
    ofTrueTypeFont titleFont;
    
    fontManager() {
    }

public:
    fontManager(const fontManager&) = delete;
    fontManager& operator=(const fontManager&) = delete;

    static fontManager& getInstance() {
        static fontManager instance; 
        return instance;
    }

    void setup(int reg_size, int title_size) {
        regularFont.load("fonts/Montserrat-Regular.otf", reg_size);
        titleFont.load("fonts/Montserrat-Bold.otf", title_size);
    }

    ofTrueTypeFont& getRegularFont() {
        return regularFont;
    }

    ofTrueTypeFont& getTitleFont() {
        return titleFont;
    }
    
};
