#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include <optional>





class MotionDetector {
private:
    ofTrueTypeFont regularFont;
    ofTrueTypeFont titleFont;

    MotionDetector() {
    }

    ofxCvGrayscaleImage grayImage;
    ofxCvContourFinder 	contourFinder;

    ofPixels pixels;

    float w_hand_rat = 0.5;
    float h_hand_rat = 0.5;
    int hand_w, hand_h, hand_x, hand_y;

    std::optional<ofxCvBlob> Hand = std::nullopt;

public:
    ofxCvGrayscaleImage grayBg;
    ofxCvGrayscaleImage grayDiff;

    MotionDetector(const MotionDetector&) = delete;
    MotionDetector& operator=(const MotionDetector&) = delete;

    static MotionDetector& getInstance() {
        static MotionDetector instance;
        return instance;
    }

    void create_cropped_img(const ofImage &cImg) {
        ofImage copy_img;
        copy_img.cropFrom(cImg, hand_x, hand_y, hand_w, hand_h);

        ofxCvColorImage colorImg;
        colorImg.allocate(hand_w, hand_h);
        colorImg.ofxCvImage::setFromPixels(copy_img.getPixels());

        grayImage = colorImg;
    }

    void setup(int camWidth, int camHeight,const ofImage& colorImg) {
        hand_w = static_cast<int>(camWidth * w_hand_rat);
        hand_x = camWidth - hand_w;
        hand_x = 0;


        hand_h = static_cast<int>(camHeight * h_hand_rat);
        hand_y = camHeight - hand_h;


        grayImage.allocate(hand_w, hand_h);
        grayBg.allocate(hand_w, hand_h);
        grayDiff.allocate(hand_w, hand_h);


        create_cropped_img(colorImg);
        grayBg = grayImage;
    }

    void update(const ofImage& colorImg) {

        create_cropped_img(colorImg);
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(80);

        contourFinder.findContours(grayDiff, 100, (hand_h * hand_w) *0.85, 10, true);

        ofxCvBlob handBlob;
        float maxArea = 0;

        for (auto& blob : contourFinder.blobs) {
            float blobWidth = blob.boundingRect.width;
            float blobHeight = blob.boundingRect.height;
            float widthRatio = blobWidth / hand_w;
            float heightRatio = blobHeight / hand_h;

            if (widthRatio >= 0.3 && heightRatio >= 0.5) {
                if (blob.area > maxArea) {
                    maxArea = blob.area;
                    handBlob = blob;
                }
            }
        }
        if (maxArea > 0) {
            Hand = handBlob;
        } else {
            Hand == std::nullopt;
        }

    };

    void keyPressed(int key){

        if(key == ' '){
            grayBg = grayImage;
            std::cout << "Updated Background\n";
        };

    };

    void draw(float x, float y, float w, float h) {
        ofSetHexColor(0xffffff);

        grayDiff.draw(x,y,w,h);

        ofFill();

        if (Hand.has_value()) {
            Hand.value().draw(x, y);
        }
    }

};