//ofApp.h


#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(255,255,255);
	ofSetVerticalSync(true);

    // setup camera grapper

    camWidth = 640;  // try to grab at this size.
	camHeight = 480;

	//get back a list of devices.
	vector<ofVideoDevice> devices = vidGrabber.listDevices();

	for(size_t i = 0; i < devices.size(); i++){
		if(devices[i].bAvailable){
			//log the device
			ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
		}else{
			//log the device and note it as unavailable
			ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
		}
	}

	vidGrabber.setDeviceID(0);
	vidGrabber.setDesiredFrameRate(30);
	vidGrabber.setup(camWidth, camHeight);
	ofSetVerticalSync(true);



    // fill with media
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

    for (int i = 0; i < std::min(static_cast<int>(dir.size()), 10); i++) {
        auto img = std::make_unique<mediaImage>(); // Use std::make_unique for safety and simplicity
        img->load(dir.getPath(i)); // Assuming dir.getPath(i) returns a std::string

        grid.addItem(std::move(img), i / 3, i % 3);
    }


    auto vid = std::make_unique<mediaVideo>(); // Use std::make_unique for safety and simplicity
    vid->load("movies/fingers.mp4"); // Assuming dir.getPath(i) returns a std::string

    grid.addItem(std::move(vid), 3, 1);

    auto feed = std::make_unique<mediaVideoFeed>(vidGrabber, camWidth, camHeight);
    grid.addItem(std::move(feed), 3, 2);

}

//--------------------------------------------------------------
void ofApp::update(){
    
    vidGrabber.update();
    state_manager.update();

}

//--------------------------------------------------------------
void ofApp::draw(){


    ofSetColor(ofColor::white);

    state_manager.draw();



}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){


}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

    state_manager.mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

    state_manager.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    state_manager.mousePressed(x, y, button);
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
        state_manager.mouseReleased(x, y, button);   
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
