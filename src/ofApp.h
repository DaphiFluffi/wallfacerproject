// ofApp.h
#pragma once

#include "ofMain.h"
#include "stateManagement.h"
#include "mediaGrid.h"
#include "cameraManager.h"
#include "mediaItems.h"
#include "fontManager.h"
#include "ioManager.h"
#include <memory>
#include <deque>

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
	void doAction(ControlActions action);


	ioManager<ofImage> image_io_manager = ioManager<ofImage>("images", { "jpg", "png" });
	ioManager<ofVideoPlayer> video_io_manager = ioManager<ofVideoPlayer>("videos", { "mp4",});
	mediaGrid baseGrid = mediaGrid(6, 3, 40, 270);


	cameraManager cam_manager = cameraManager(640, 480);
	StateManager& state_manager = StateManager::getInstance(image_io_manager, cam_manager);
	MotionDetector& motion_detector = MotionDetector::getInstance();

	std::map<std::string, bool> cmdArgs;
};
