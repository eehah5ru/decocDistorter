#include "Video.h"
#include "constants.hpp"
#include "ofEvent.h"
#include "ofEventUtils.h"
#include "ofEvents.h"
#include "ofGraphics.h"
#include "ofMain.h"
#include "ofxCvBlob.h"

Video::Video() {
}

Video::~Video() {
  ofRemoveListener(ofEvents().keyPressed, this, &Video::keyPressed);
}

void Video::setup() {
  _grabber.setVerbose(true);
  _grabber.setup(_width, _height);

  _colorImg.allocate(_width, _height);
  _grayImage.allocate(_width, _height);
  _grayBg.allocate(_width, _height);
  _grayDiff.allocate(_width, _height);

  _bLearnBakground = true;
  _threshold = 80;

  ofAddListener(ofEvents().keyPressed, this, &Video::keyPressed);
}

void Video::update() {
  _grabber.update();

  if (!_grabber.isFrameNew()) {
    return;
  }

  _colorImg.setFromPixels(_grabber.getPixels());

  // FIXME! copy
  // and conversion to gray
  _grayImage = _colorImg;
  if (_bLearnBakground) {
    _grayBg = _grayImage;
    _bLearnBakground = false;
  }

  _grayDiff.absDiff(_grayBg, _grayImage);
  _grayDiff.threshold(_threshold);

    // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
    // also, find holes is set to true so we will get interior contours as well....
  _contourFinder.findContours(_grayDiff, 20, (_width*_height)/3, 10, true);	// find holes

  // debugContours();

}

void Video::draw() {
  ofPushStyle();
  ofSetColor(ofColor::wheat);
  _grabber.draw(20, 20);

  ofFill();
  ofSetColor(ofColor::deepPink);

  _contourFinder.draw(20, 20);
  ofPopStyle();
}

void Video::keyPressed(ofKeyEventArgs &key) {
  switch (key.key){
    case ' ':
      _bLearnBakground = true;
      break;
    case '+':
      _threshold ++;
      if (_threshold > 255) _threshold = 255;
      break;
    case '-':
      _threshold --;
      if (_threshold < 0) _threshold = 0;
      break;
  }
}

//
// get found contours
//
vector<ofxCvBlob> &Video::contours() {
  return _contourFinder.blobs;
}

//
// print contours info
//
void Video::debugContours() {
  stringstream msg;

  msg << "Contours info:\n";
  msg << "numBlobs: " << _contourFinder.nBlobs << endl;

  int i = 0;
  for (ofxCvBlob blob : _contourFinder.blobs) {
    i++;
    msg << "Blob " << i << "- points: " << blob.nPts << endl;
    // for (ofDefaultVec3 p; )
  }

  std::cerr << msg.str();
}
