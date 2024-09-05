#include "ofApp.h"
#include "Video.h"
#include "ofAppGlutWindow.h"
#include "ofAppRunner.h"
#include "ofColor.h"
#include "ofEventUtils.h"
#include "ofEvents.h"
#include "ofGraphics.h"
#include "ofGraphicsBaseTypes.h"
#include "ofPoint.h"
#include <memory>


// ofApp::ofApp()
//   : _video({1280, 720}) {
// }

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetLogLevel(OF_LOG_VERBOSE);

  _video.setup();
  _diagram.setup();

  _compositeRecorder.setup("frames/composite/frame_{:0>8}.png");
  _diagramRecorder.setup("frames/diagram/frame_{:0>8}.png"); 
  
  
  _communicator.setup();

  ofAddListener(_communicator.onMapUpdated, this, &ofApp::onMapUpdated);
  ofAddListener(_diagram.onUpdated, this, &ofApp::onDiagramUpdated);
  
}  

void ofApp::updateSync() {
  _video.update();
  sendShakeCommandSync();
  _diagram.loadDiagram();
  _diagram.update();

}

void ofApp::updateAsync() {
  _video.update();
  _communicator.updateAsync();
  _diagram.update();
}
//--------------------------------------------------------------
void ofApp::update(){
  if (APP_ASYNC_MODE) {
    updateAsync();
  } else {
    updateSync();
  }
  // cerr << "ww: " << ofGetWindowWidth() << " wh: " << ofGetWindowHeight() << " w: " << ofGetWidth() << " h: " << ofGetHeight() << endl;
}

//--------------------------------------------------------------
void ofApp::draw(){
  // ofBackground(ofColor::hotPink);
  _video.drawVideo();
  _diagram.draw();
  _compositeRecorder.recordFrame();
  // LOG_APP_VERBOSE() << "1";

  // ofEnableAlphaBlending();
  // ofClear(ofColor::red);
  // i.get()->draw(0, 0);
  shared_ptr<ofFloatImage> i = _diagram.getImage();  
  _diagramRecorder.recordFrame(i);
  // LOG_APP_VERBOSE() << "2";   
  _video.drawMetadata();
  // LOG_APP_VERBOSE() << "3"; 
  
  
  _compositeRecorder.draw(ofGetWindowWidth() - 40, 40);
  _diagramRecorder.draw(ofGetWindowWidth() - 80, 40);
  
  
}

//--------------------------------------------------------------
void ofApp::exit(){
  _communicator.exit();
  _compositeRecorder.exit();
}

//
//
// custom funcs
//
// 

void ofApp::sendShakeCommandAsync() {
  _communicator.sendFocus(_video.getFocus(), _video.getFocusRadius());
  _communicator.sendShakePositionsNearFocus();
}

//
// send and wait till results
// 
void ofApp::sendShakeCommandSync() {
  _communicator.sendFocus(_video.getFocus(), _video.getFocusRadius());
  _communicator.sendShakePositionsNearFocus();
  _communicator.waitUnillNextIncomingMessage();
}

//
// 
// custom events
//
//

//
// on map updated on disk - got osc message
// 
void ofApp::onMapUpdated (int &isUpdated) {
  // if NOT ASYNC_MODE - ignore
  if (APP_ASYNC_MODE) {
    _diagram.loadDiagram();
  }
}

// on map updated in app
// 
void ofApp::onDiagramUpdated (int &isUpdated) {
  // _communicator.sendContours(_video.polylineContours());  
  // _communicator.sendShakePositionsInContours();

  if (!APP_ASYNC_MODE) {
    return;
  }
  // only in ASYNC MODE
  sendShakeCommandAsync();
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


