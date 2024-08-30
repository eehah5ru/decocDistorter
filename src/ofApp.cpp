#include "ofApp.h"
#include "Video.h"
#include "ofAppRunner.h"
#include "ofColor.h"
#include "ofEventUtils.h"
#include "ofEvents.h"
#include "ofGraphics.h"


// ofApp::ofApp()
//   : _video({1280, 720}) {
// }

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetLogLevel(OF_LOG_VERBOSE);

  _video.setup();
  _diagram.setup();

  _communicator.setup();

  ofAddListener(_communicator.onMapUpdated, this, &ofApp::onMapUpdated);
  ofAddListener(_diagram.onUpdated, this, &ofApp::onDiagramUpdated);
  
}  

//--------------------------------------------------------------
void ofApp::update(){
  _video.update();
  _communicator.update();
  _diagram.update();
  // cerr << "ww: " << ofGetWindowWidth() << " wh: " << ofGetWindowHeight() << " w: " << ofGetWidth() << " h: " << ofGetHeight() << endl;
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofBackground(ofColor::hotPink);
  _video.drawVideo();
  _diagram.draw();
  _screen.grabScreen(0, 0 , ofGetWidth(), ofGetHeight());
  _screen.save(std::format("frames/frame_{:0>8}.png", ofGetFrameNum()));
  _video.drawMetadata();
  
}

//--------------------------------------------------------------
void ofApp::exit(){
  _communicator.exit();
}

//
// 
// custom events
//
//

//
// on map updated on disk
// 
void ofApp::onMapUpdated (int &isUpdated) {
  _diagram.loadDiagram();
}

// on map updated in app
// 
void ofApp::onDiagramUpdated (int &isUpdated) {
  _communicator.sendContours(_video.polylineContours());  
  _communicator.sendShakePositions();
}

//
// standard events
// 

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
  // _communicator.sendMouse(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

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


