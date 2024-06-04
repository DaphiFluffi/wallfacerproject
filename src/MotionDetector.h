#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include <optional>
#include <tuple>


enum class ControlActions {
    FILL,
    PICK,
    MOVING_UP,
};

ofRectangle translate(ofRectangle rectangle, float x, float y);

ofRectangle scaleFromCenter(ofRectangle rect, float factor);


class MotionActionBox {
    int ticks = 0;
    int maxTicks = 5;
    float draw_size = 1.0;
public:
    float center_x, center_y, width; // cordinates on gray image not on draw
    ControlActions action;
    ofColor color;

    MotionActionBox(float x, float y, float s, ControlActions action, ofColor color) : center_x(x),center_y(y), width(s), action(action), color(color) {
    }

    std::tuple<float, float> get_center() const
    {
        return {center_x, center_y};
    };

    ofRectangle get_bounding_box(const float size) const
    {
        auto [center_x, center_y] = get_center();
        float x = center_x - width * size / 2;
        float y = center_y - width * size / 2;
        return ofRectangle(x, y, width * size, width* size);
    }


    bool contains(const float x,const  float y) const
    {
        const auto box = get_bounding_box(draw_size);
        return box.inside(x, y);
    }

    void draw(float x, float y, float w, float h) {

        draw_size = (0.5/ (maxTicks - 1)) * ticks + 1.0;

        ofSetColor(color);
        ofFill();
        auto box = get_bounding_box(draw_size);
        ofDrawRectangle(x + box.x * w, y + box.y * h, box.width * w, box.height * h);
        ofNoFill();
    }

    std::optional<ControlActions> hover_over(float x, float y) {

        if (contains(x, y)) {

            cout << "hovering " << ticks << endl;

            if (ticks == maxTicks) {
                return action;
            }

            ticks++;
        } else {
            ticks = 0;
        }
        return std::nullopt;
    }

};


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
    ofRectangle centerHandRect;

    vector<tuple<float,float, ControlActions, ofColor>> motionBoxes = {
            {0.15, 0.52, ControlActions::FILL, ofColor(0,255,0)},
            {0.85, 0.52, ControlActions::PICK, ofColor(0,0,255)},
            {0.5, 0.7, ControlActions::MOVING_UP, ofColor(0,255,255)},
    };

    vector<MotionActionBox> boxes;

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

        //setup Motion boxes
        for (auto info : motionBoxes) {
            auto [x, y, action, color] = info;
            boxes.push_back(MotionActionBox(x * hand_w, y * hand_h, 0.1 * hand_h, action, color));
        }


    }

    std::optional<ControlActions> update(const ofImage& colorImg) {

        create_cropped_img(colorImg);
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(80);

        contourFinder.findContours(grayDiff, 100, (hand_h * hand_w) *0.9, 10, true);

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
        if (maxArea > 50) {
            Hand = handBlob;
            centerHandRect = scaleFromCenter(handBlob.boundingRect, 0.1);
        } else {
            Hand == std::nullopt;
        }


        if (!Hand.has_value()) return  std::nullopt;

        auto center = centerHandRect.getCenter();


        for (auto& box : boxes) {

            auto val = box.hover_over(center.x, center.y);

            if (val.has_value()) {
                return val;
            }
        }

        return std::nullopt;
    }


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

        float stretch_w = w / hand_w;
        float stretch_h = h / hand_h;

        if (Hand.has_value()) {

            auto blob = Hand.value();

            ofPushStyle();
            ofNoFill();
            ofSetHexColor(0x00FFFF);
            ofBeginShape();
            for (int i = 0; i < blob.nPts; i++){
                ofVertex(x + blob.pts[i].x  * stretch_w, y + blob.pts[i].y * stretch_h);
            }
            ofEndShape(true);
            ofSetHexColor(0xff0099);
            ofDrawRectangle(x + blob.boundingRect.x * stretch_w, y + blob.boundingRect.y * stretch_h, blob.boundingRect.width  * stretch_w, blob.boundingRect.height * stretch_h);
            ofPopStyle();

            ofFill();
            ofSetColor(255,0,0);
            ofDrawRectangle(x + centerHandRect.x * stretch_w, y + centerHandRect.y * stretch_h , centerHandRect.width * stretch_w, centerHandRect.height * stretch_h);
            ofNoFill();
        }

        for (auto& box : boxes) {
            box.draw(x, y, stretch_w, stretch_h);
        }
    }

};