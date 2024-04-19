// ofApp.h
#pragma once

#include "ofMain.h"
#include "stateManagement.h"
#include "mediaGrid.h"
#include "cameraManager.h"
#include "mediaItems.h"

class ofApp : public ofBaseApp
{

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofDirectory dir;
	vector<ofImage> images;

	mediaGrid grid = mediaGrid(4, 3, 30, 200);
	StateManager state_manager = StateManager(&grid);

	cameraManager cam_manager = cameraManager(640, 480);
};
