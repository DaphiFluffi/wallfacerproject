#pragma once

#include "ofMain.h"

class cameraManager
{

    ofVideoGrabber vidGrabber;
    ofImage img;
    // ofxCvHaarFinder finder;s
    int camWidth, camHeight;

public:
    cameraManager(int width, int height) : camWidth(width), camHeight(height){};

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

        // finder.setup("haarcascade_frontalface_default.xml");
	    // finder.findHaarObjects(img);
    };

    void update(){
        vidGrabber.update();
        if(vidGrabber.isFrameNew()){};
    };

    void draw(float x, float y, float w, float h){
        vidGrabber.draw(x,y,w,h);

    //     	ofNoFill();
	// for(unsigned int i = 0; i < finder.blobs.size(); i++) {
	// 	ofRectangle cur = finder.blobs[i].boundingRect;
	// 	ofDrawRectangle(cur.x, cur.y, cur.width, cur.height);
	// }
    };
};