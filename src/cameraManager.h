#pragma once

#include "ofMain.h"
#include "ofxCvHaarFinder.h"
#include "ofxOpenCv.h"
#include "CV_Stuff.h"
#include "MotionDetector.h"

class cameraManager
{

    ofVideoGrabber vidGrabber;
    ofImage img;
    int camWidth, camHeight;
    int detect_every_n;
    int steps;
    int thresh;
    vector<ofRectangle> faces;


public:
    cameraManager(int width, int height, int det_every_n = 10, int threshold = 80) 
        : camWidth(width), camHeight(height), steps(0), 
        detect_every_n(det_every_n), thresh(threshold) {
    };

    void setup()
    {


        // get back a list of devices.
        vector<ofVideoDevice> devices = vidGrabber.listDevices();

        for (size_t i = 0; i < devices.size(); i++)
        {
            if (devices[i].bAvailable)
            {
                // log the device
                ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
            }
            else
            {
                // log the device and note it as unavailable
                ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
            }
        }

        vidGrabber.setDeviceID(0);
        vidGrabber.setDesiredFrameRate(30);
        vidGrabber.setup(camWidth, camHeight);
        ofSetVerticalSync(true);
        vidGrabber.update();

        MotionDetector::getInstance().setup(camWidth, camHeight, vidGrabber.getPixels());
    };



     std::optional<ControlActions> update(){
        vidGrabber.update();
        ofPixels pixels = vidGrabber.getPixels();

        if(steps % detect_every_n == 0){

            faces = FaceFinder::getInstance().detect_faces(pixels);
        };

        steps++;

        return MotionDetector::getInstance().update(vidGrabber.getPixels());
    };

    void draw(float x, float y, float w, float h){
        vidGrabber.draw(x,y,w,h);
        ofNoFill();

        // this only has the original coordinates of the image

        for (const auto &cur : faces)
        {
            float new_x = (cur.x/camWidth)*w + x;
        float new_y = (cur.y/camHeight)*h + y;
        float new_w = cur.width / camWidth * w;
        float new_h = cur.height / camHeight * h;

		ofDrawRectangle(new_x, new_y, new_w, new_h);
        }

        ofFill();
   

    };

    ofxCvColorImage get_frame() {
        ofxCvColorImage colorImg;
        colorImg.ofxCvImage::setFromPixels(vidGrabber.getPixels());
        return colorImg;
    }


    void keyPressed(int key){
        std::cerr << "keypressed in cam manager depreated" << endl;
    };
};