#include "ofMain.h"
#include "settings.h"
#include "ofApp.h"

//========================================================================
int main( ){
  Magick::InitializeMagick(nullptr);

	//Use ofGLFWWindowSettings for more options like multi-monitor fullscreen
	ofGLWindowSettings settings;
	settings.setSize(APP_WIDTH, APP_HEIGHT);
	settings.windowMode = OF_WINDOW; //can also be OF_FULLSCREEN

	auto window = ofCreateWindow(settings);

	ofRunApp(window, std::make_shared<ofApp>());
	ofRunMainLoop();

}
