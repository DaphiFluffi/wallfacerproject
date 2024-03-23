//ofApp.cpp
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(255,255,255);
	ofSetVerticalSync(true);
	frameByframe = false;

	// Uncomment this to show movies with alpha channels
	// fingerMovie.setPixelFormat(OF_PIXELS_RGBA);

	// fingerMovie.load("movies/fingers.mp4");
	// fingerMovie.setLoopState(OF_LOOP_NORMAL);
	// fingerMovie.play();

	// pause_rec = ofRectangle(400,400,20,20);

	dir.listDir("images/of_logos/");
	dir.allowExt("jpg");
	dir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order

	//allocate the vector to have as many ofImages as files
	if( dir.size() ){
		images.assign(dir.size(), ofImage());
	}

    std::cout << "\n---------------------------------------------\nimages " << dir.size() << endl << dir.path() << endl;;

    // grid = mediaGrid(3, 4, 20, 200);

	// you can now iterate through the files and load them into the ofImage vector

    for (int i = 0; i < std::min(static_cast<int>(dir.size()), 12); i++) {
        auto img = std::make_unique<mediaImage>(); // Use std::make_unique for safety and simplicity
        img->load(dir.getPath(i)); // Assuming dir.getPath(i) returns a std::string

        grid.addItem(std::move(img), i / 3, i % 3);
    }


	// camWidth = 640;  // try to grab at this size.
	// camHeight = 480;

	// //get back a list of devices.
	// vector<ofVideoDevice> devices = vidGrabber.listDevices();

	// for(size_t i = 0; i < devices.size(); i++){
	// 	if(devices[i].bAvailable){
	// 		//log the device
	// 		ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
	// 	}else{
	// 		//log the device and note it as unavailable
	// 		ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
	// 	}
	// }

	// vidGrabber.setDeviceID(0);
	// vidGrabber.setDesiredFrameRate(30);
	// vidGrabber.setup(camWidth, camHeight);

	// videoInverted.allocate(camWidth, camHeight, OF_PIXELS_RGB);
	// videoTexture.allocate(videoInverted);
	// ofSetVerticalSync(true);


}

//--------------------------------------------------------------
void ofApp::update(){


    grid.update();

	// fingerMovie.update();


	// if(!frameByframe) vidGrabber.update();

	// if(vidGrabber.isFrameNew()){
	// 	ofPixels & pixels = vidGrabber.getPixels();
	// 	for(size_t i = 0; i < pixels.size(); i++){
	// 		//invert the color of the pixel
	// 		videoInverted[i] = 255 - pixels[i];
	// 	}
	// 	//load the inverted pixels
	// 	videoTexture.loadData(videoInverted);
	// }
}

//--------------------------------------------------------------
void ofApp::draw(){


	// ofSetHexColor(0xFFFFFF);
    ofSetColor(ofColor::white);

    grid.draw();

	// fingerMovie.draw(20,500);


	// if (dir.size() > 0){
	// 	ofSetColor(ofColor::white);
	// 	images[currentImage].draw(20,20);

	// 	ofSetColor(ofColor::gray);
	// 	string pathInfo = dir.getName(currentImage) + " " + dir.getPath(currentImage) + "\n\n" +
	// 		"press any key to advance current image\n\n" +
	// 		"many thanks to hikaru furuhashi for the OFs";
	// 	ofDrawBitmapString(pathInfo, 20, images[currentImage].getHeight() + 80);
	
	
	// 	ofSetColor(ofColor::gray);
	// 	for(int i = 0; i < (int)dir.size(); i++){
	// 		if(i == currentImage) {
	// 			ofSetColor(ofColor::red);
	// 		}	else {
	// 			ofSetColor(ofColor::black);
	// 		}
	// 		string fileInfo = "file " + ofToString(i + 1) + " = " + dir.getName(i);
	// 		ofDrawBitmapString(fileInfo, 500,i * 20 + 50);
	// 	}
	// }

	// int y_offset = 250;
	
	// ofSetHexColor(0x000000);
	// ofDrawBitmapString("press f to change",500,320 + y_offset);
	// if(frameByframe) ofSetHexColor(0xCCCCCC);
	// ofDrawBitmapString("mouse speed position",500,340 + y_offset);
	// if(!frameByframe) ofSetHexColor(0xCCCCCC); else ofSetHexColor(0x000000);
	// ofDrawBitmapString("keys <- -> frame by frame " ,500,340 + y_offset);
	// ofSetHexColor(0x000000);

	// ofDrawBitmapString("frame: " + ofToString(fingerMovie.getCurrentFrame()) + "/"+ofToString(fingerMovie.getTotalNumFrames()),500,380 + y_offset);
	// ofDrawBitmapString("duration: " + ofToString(fingerMovie.getPosition()*fingerMovie.getDuration(),2) + "/"+ofToString(fingerMovie.getDuration(),2),500,400 + y_offset);
	// ofDrawBitmapString("speed: " + ofToString(fingerMovie.getSpeed(),2),500,420 + y_offset);

	// if(fingerMovie.getIsMovieDone()){
	// 	ofSetHexColor(0xFF0000);
	// 	ofDrawBitmapString("end of movie",500,440 + y_offset);
	// }

	// ofSetColor(255, 0, 0);
	// ofDrawRectangle(pause_rec);




	// ofSetHexColor(0xffffff);
	// vidGrabber.draw(700, 20);
	// videoTexture.draw(20 + camWidth, 20, camWidth, camHeight);

}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
	switch(key){
		case 'f':
			frameByframe=!frameByframe;
			fingerMovie.setPaused(frameByframe);
		break;
		case OF_KEY_LEFT:
			fingerMovie.previousFrame();
		break;
		case OF_KEY_RIGHT:
			fingerMovie.nextFrame();
		break;
		case '0':
			fingerMovie.firstFrame();
		break;
	}


}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	// if(!frameByframe){
	// 	int width = ofGetWidth();
	// 	float pct = (float)x / (float)width;
	// 	float speed = (2 * pct - 1) * 5.0f;
	// 	fingerMovie.setSpeed(speed);
	// }

    grid.mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if(!frameByframe){
		int width = ofGetWidth();
		float pct = (float)x / (float)width;
		fingerMovie.setPosition(pct);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

	if( pause_rec.inside(x,y)){
		
		frameByframe=!frameByframe;
		fingerMovie.setPaused(frameByframe);
	}


	if (dir.size() > 0){
		currentImage++;
		currentImage %= dir.size();
	}
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if(!frameByframe){
		fingerMovie.setPaused(false);
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
