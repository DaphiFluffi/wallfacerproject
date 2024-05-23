//
// Created by felix on 26.04.24.
//
#pragma once

#include "ofMain.h"
#include <tuple>
#include "ofxCvColorImage.h"
#include <optional>



ofxCvColorImage ofImage_to_CV(ofImage& img);
ofImage CV_to_ofImage(ofxCvColorImage& img);

tuple<float, float, float, float> get_centerted_box(float aspectRatio = 1.0f);
ofRectangle get_bounding_rect(float aspectRatio = 1.0f);