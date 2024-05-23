//
// Created by felix on 26.04.24.
//
#include "utils.h"



ofxCvColorImage ofImage_to_CV(ofImage& img){
    ofxCvColorImage colorImg;
    colorImg.allocate(img.getWidth(), img.getHeight());
    colorImg.setFromPixels(img.getPixels());
    return colorImg;
};

ofImage CV_to_ofImage(ofxCvColorImage& img)
{
    ofImage ofImg;
    ofImg.allocate(img.getWidth(), img.getHeight(), OF_IMAGE_COLOR);
    ofImg.setFromPixels(img.getPixels());
    return ofImg;
};

tuple<float,float,float,float> get_centerted_box(float aspectRatio){
    int screenWidth = ofGetWindowWidth();
    int screenHeight = ofGetWindowHeight();

    float drawWidth, drawHeight;
    if (aspectRatio > 1)
    {
        drawWidth = screenHeight * aspectRatio;
        drawHeight = screenHeight;
    }
    else
    {
        drawWidth = screenWidth;
        drawHeight = screenWidth / aspectRatio;
    }

    if (drawWidth > screenWidth)
    {
        drawWidth = screenWidth;
        drawHeight = drawWidth / aspectRatio;
    }
    if (drawHeight > screenHeight)
    {
        drawHeight = screenHeight;
        drawWidth = drawHeight * aspectRatio;
    }

    float drawX = max((screenWidth - drawWidth) / 2, 0.0f);
    float drawY = max((screenHeight - drawHeight) / 2, 0.0f);
    return make_tuple(drawX, drawY, drawWidth, drawHeight);

};

ofRectangle get_bounding_rect(float aspectRatio){
    auto [drawX, drawY, drawWidth, drawHeight] = get_centerted_box(aspectRatio);
    return ofRectangle(drawX, drawY, drawWidth, drawHeight);
};
