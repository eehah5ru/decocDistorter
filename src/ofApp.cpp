#include "ofApp.h"
#include "ofColor.h"
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

  _communicator.sendContours(_video.contours());
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofBackground(ofColor::hotPink);
   _video.draw();
  _diagram.draw();
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
  _communicator.sendMouse(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
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
