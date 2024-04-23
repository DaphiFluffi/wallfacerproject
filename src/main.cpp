#include "ofMain.h"
#include "ofApp.h"
#include <map>
#include <string>

//========================================================================
int main(int argc, char *argv[]) {

    // Map to hold command line arguments
    std::map<std::string, bool> cmdArgs;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--gmd") {
            cmdArgs[arg] = true;
        }
    }
	//Use ofGLFWWindowSettings for more options like multi-monitor fullscreen
	ofGLWindowSettings settings;
	settings.setSize(1920, 1080);
	settings.windowMode = OF_WINDOW; //can also be OF_FULLSCREEN

	auto window = ofCreateWindow(settings);

	auto app = make_shared<ofApp>();

	app->cmdArgs = cmdArgs;
	
	ofRunApp(window, app);
	ofRunMainLoop();

}
