#include "ofApp.h"

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

  _communicator.send(_video.contours());
}

//--------------------------------------------------------------
void ofApp::draw(){
  //  _video.draw();
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
  _communicator.send(x, y);
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
