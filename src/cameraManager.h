#pragma once

#include "ofMain.h"
#include "ofxCvHaarFinder.h"

class cameraManager
{

    ofVideoGrabber vidGrabber;
    ofImage img;
    ofxCvHaarFinder finder;
    int camWidth, camHeight;
    int detect_every_n;
    int steps;
    vector<ofRectangle> faces;

public:
    cameraManager(int width, int height, int det_every_n = 10) : camWidth(width), camHeight(height), steps(0), detect_every_n(det_every_n){};

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

        finder.setup("haarcascade_frontalface_default.xml");
    };

    void update(){
        vidGrabber.update();
        if(steps % detect_every_n == 0){

	    finder.findHaarObjects(vidGrabber.getPixels());

        faces.clear();
        for(unsigned int i = 0; i < finder.blobs.size(); i++) {
		    faces.push_back(finder.blobs[i].boundingRect);
        }


        };
        steps++;
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
};