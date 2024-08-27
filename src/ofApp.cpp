#include "ofApp.h"
#include "ofAppRunner.h"
#include "ofColor.h"
#include "ofEvents.h"
#include "ofGraphics.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetLogLevel(OF_LOG_VERBOSE);

  _video.setup();
  _diagram.setup();

  _communicator.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
  _video.update();
  _diagram.update();
  // cerr << "ww: " << ofGetWindowWidth() << " wh: " << ofGetWindowHeight() << " w: " << ofGetWidth() << " h: " << ofGetHeight() << endl;
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofBackground(ofColor::hotPink);
   _video.drawVideo();
  _diagram.draw();
  _video.drawMetadata();
}

//--------------------------------------------------------------
void ofApp::exit(){
  _communicator.exit();
}

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
  _communicator.sendContours(_video.boundingBoxes());  
  _communicator.sendShakePositions();
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
