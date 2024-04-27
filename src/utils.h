//
// Created by felix on 26.04.24.
//
#pragma once

#include "ofMain.h"
#include <tuple>



tuple<float, float, float, float> get_centerted_box(float aspectRatio = 1.0f);
ofRectangle get_bounding_rect(float aspectRatio = 1.0f);