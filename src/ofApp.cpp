// ofApp.h

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofBackground(255, 255, 255);

    cam_manager.setup();
    image_io_manager.setup();
    video_io_manager.setup();

    // fill grid 
    for (int i = 0; i < std::min(static_cast<int>(image_io_manager.size()), 10); i++)
    {
        auto img = std::make_unique<mediaImage>(image_io_manager.getData(i)); 

        grid.addItem(std::move(img), i / 3, i % 3);
    }

    auto vid = std::make_unique<mediaVideo>(video_io_manager.getData(0)); 

    grid.addItem(std::move(vid), 3, 1);

    auto feed = std::make_unique<mediaVideoFeed>(cam_manager);
    grid.addItem(std::move(feed), 3, 2);

    if (cmdArgs.find("--gmd") != cmdArgs.end())
    {
        std::cout << "Generating metadata" << std::endl;

        image_io_manager.updateMetadata();
        video_io_manager.updateMetadata();
    }
}

//--------------------------------------------------------------
void ofApp::update()
{

    cam_manager.update();
    state_manager.update();
}

//--------------------------------------------------------------
void ofApp::draw()
{

    ofSetColor(ofColor::white);

    state_manager.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{

    state_manager.mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

    state_manager.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

    state_manager.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

    state_manager.mouseReleased(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
}
