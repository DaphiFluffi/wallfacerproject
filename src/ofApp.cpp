// ofApp.h

#include "ofApp.h"
#include "searchManager.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofBackground(255, 255, 255);


    // ------------- general setups ----------------
    cam_manager.setup();
    image_io_manager.setup();
    video_io_manager.setup();


    // add base window
    state_manager.addWindow(std::move(std::make_unique<DisplayWindow>(&baseGrid)));


    fontManager& font_manager = fontManager::getInstance();
    FaceFinder::getInstance().setup();
    font_manager.setup(12, 20);
    SearchManager::getInstance().setup(&image_io_manager, &video_io_manager);


    // ------------- fill grid -------------
    for (int i = 0; i < std::min(static_cast<int>(image_io_manager.size()), 12); i++)
    {
        auto img = std::make_unique<mediaImage>(image_io_manager.getData(i)); 

        baseGrid.addItem(std::move(img));
    }

    baseGrid.addItem(std::move(std::make_unique<collectorItem>()));


    auto a_collector = std::make_unique<collectorItem>();
    baseGrid.addItem(std::move(a_collector));


    auto vid = std::make_unique<mediaVideo>(video_io_manager.getData(0)); 
    baseGrid.addItem(std::move(vid));

    auto feed = std::make_unique<mediaFeed<cameraManager>>(cam_manager);
    baseGrid.addItem(std::move(feed));
    

    auto background_feed = std::make_unique<mediaFeed<ofxCvGrayscaleImage>>(MotionDetector::getInstance().grayBg);
    baseGrid.addItem(std::move(background_feed));


    auto diff_feed = std::make_unique<mediaFeed<MotionDetector>>(motion_detector);
    baseGrid.addItem(std::move(diff_feed));

    // ------------- generate metadata -------------

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


    MotionDetector::getInstance().keyPressed(key);

    state_manager.keyPressed(key);
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
