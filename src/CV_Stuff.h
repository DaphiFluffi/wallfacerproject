//
// Created by felix on 10.05.24.
//

#pragma once

#include "ofMain.h"
#include "ofxCvHaarFinder.h"
#include "ofxOpenCv.h"


class ConvManager {
private:
    ofxCvHaarFinder finder;

    ConvManager() {
    }

public:
    ConvManager(const ConvManager&) = delete;
    ConvManager& operator=(const ConvManager&) = delete;

    static ConvManager& getInstance() {
        static ConvManager instance;
        return instance;
    }

    void setup() {
        // load kernels
    }


    };


class FaceFinder {
private:
    ofxCvHaarFinder finder;

    FaceFinder() {
    }

public:
    FaceFinder(const FaceFinder&) = delete;
    FaceFinder& operator=(const FaceFinder&) = delete;

    static FaceFinder& getInstance() {
        static FaceFinder instance;
        return instance;
    }

    void setup() {
            finder.setup("haarcascade_frontalface_default.xml");
    }

    vector<ofRectangle> detect_faces(ofPixels& pixels){
        finder.findHaarObjects(pixels);

        vector<ofRectangle> faces;

        for(unsigned int i = 0; i < finder.blobs.size(); i++) {
            faces.push_back(finder.blobs[i].boundingRect);
        }

        return faces;
    };


};

