#include "Video.h"
#include "Communicator.h"
#include "constants.hpp"
#include "ofEvent.h"
#include "ofEventUtils.h"
#include "ofEvents.h"
#include "ofGraphics.h"
#include "ofMain.h"
#include "ofPath.h"
#include "ofPolyline.h"
#include "ofRectangle.h"
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
  _contourFinder.findContours(_grayDiff, 20, (_width*_height)/3, 10, false, true);

  // debugContours();

}

void Video::drawVideo() {
  _grabber.draw(0, 0);
}

void Video::drawMetadata() {
  ofPushStyle();
  ofSetColor(ofColor::deepPink);

  _contourFinder.draw(0, 0);
  ofPopStyle();
}

void Video::drawAll() {
  drawVideo();
  drawMetadata();
}

void Video::keyPressed(ofKeyEventArgs &key) {
  switch (key.key){
    case ' ':
      _bLearnBakground = true;
      break;
    case '+':
      LOG_COMM_VERBOSE() << "theshold++";
      _threshold ++;
      if (_threshold > 255) _threshold = 255;
      break;
    case '-':
      LOG_COMM_VERBOSE() << "threshold--";
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
// convex hulls for contours
//
vector<ofPolyline> Video::convexHulls() {
  vector<ofPolyline> r;

  for (auto contour : contours()) {
    vector<ofPoint> pts;
    ofPolyline p;

    for (auto point : contour.pts) {
      pts.push_back({point.x, point.y});
    }

    for (auto point :_convexHull.getConvexHull(pts)) {
      p.addVertex({point.x, point.y, 0});
    }

    r.push_back(p);
  }

  return r;
}

//
// bounding boxes for contours
//
vector<ofPolyline> Video::boundingBoxes() {
  vector<ofPolyline> r;

  for (auto contour : contours()) {
    vector<ofPoint> pts;
    ofPolyline p;

    p.addVertex(contour.boundingRect.getTopLeft());
    p.addVertex(contour.boundingRect.getTopRight());
    p.addVertex(contour.boundingRect.getBottomRight());
    p.addVertex(contour.boundingRect.getBottomLeft());

    r.push_back(p);
  }

  return r;


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
