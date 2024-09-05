#include "Video.h"
#include "Communicator.h"
#include "constants.hpp"
#include "ofAppRunner.h"
#include "ofColor.h"
#include "ofEvent.h"
#include "ofEventUtils.h"
#include "ofEvents.h"
#include "ofFileUtils.h"
#include "ofGraphics.h"
#include "ofGraphicsConstants.h"
#include "ofMain.h"
#include "ofMath.h"
#include "ofPath.h"
#include "ofPixels.h"
#include "ofPoint.h"
#include "ofPolyline.h"
#include "ofRandomDistributions.h"
#include "ofRectangle.h"
#include "ofUtils.h"
#include "ofxCvBlob.h"
#include "settings.h"
#include <cmath>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <variant>

void not_implemented() {
  throw std::logic_error("myFunction is not implemented yet.");
}
// //
// //
// //
// void NullVideoSource::setup() {
//   not_implemented();
// }

// void NullVideoSource::update() {
//   not_implemented();
// }

// void NullVideoSource::draw(int x, int y) {
//   not_implemented();
// }

// int NullVideoSource::getWidth() {
//   not_implemented();
// }

// int NullVideoSource::getHeight() {
//   not_implemented();
// }

// bool NullVideoSource::isFrameNew() {
//   not_implemented();
// }

// ofPixels& getPixels() {
//   not_implemented();
// }
//
// CameraSource
//
CameraSource::CameraSource() {}

CameraSource::~CameraSource() {};

size_t CameraSource::getWidth() {
  return _width;
}

size_t CameraSource::getHeight() {
  return _height;
}

void CameraSource::setup() {
  _grabber.setVerbose(true);
  _grabber.setup(_width, _height);
}

void CameraSource::update() {
  _grabber.update();
}

void CameraSource::draw(int x, int y) {
  _grabber.draw(x, y);
}

bool CameraSource::isFrameNew() {
  return _grabber.isFrameNew();
}

ofPixels& CameraSource::getPixels() {
  return _grabber.getPixels();
}

ofPoint CameraSource::getFocus() {
  return {0.5, 0.5};
}

float CameraSource::getFocusRadius() {
  return 0.5;
}

//
// file source
//
// FileSource::FileSource() {}

// FileSource::~FileSource() {};

size_t FileSource::getWidth() {
  return _width;
}

size_t FileSource::getHeight() {
  return _height;
}

void FileSource::setup() {
  string path = ofGetEnv("VIDEO_FILE_NAME", "");

  if (path.empty()) {
    throw std::invalid_argument("VIDEO_FILE_NAME is not set");
  }

  string dataPath = ofToDataPath(path);
  ofFile file{dataPath};

  if (!file.exists()) {
    throw std::invalid_argument(std::format("video file does not exist: {}", dataPath));
  }

  _player.load(dataPath);
  _player.play();

  // _grabber.setVerbose(true);
  // _grabber.setup(_width, _height);
}

void FileSource::update() {
  _player.update();
}

void FileSource::draw(int x, int y) {
  _player.draw(x, y);
  // _grabber.draw(x, y);
}

bool FileSource::isFrameNew() {
  return _player.isFrameNew();
}

ofPixels& FileSource::getPixels() {
  return _player.getPixels();
  // return ofPixels();
  // return _grabber.getPixels();
}

ofPoint FileSource::getFocus() {
  return {0.5, 0.5};
}

float FileSource::getFocusRadius() {
  return 0.5;
}

//
// CIRCLE SOURCE
//
void CircleSource::setup() {
  _fbo.allocate(APP_WIDTH, APP_HEIGHT);
  _pixels.allocate(APP_WIDTH, APP_HEIGHT, OF_IMAGE_COLOR);


  createRandomPolyline(getRandomPoint());

  // set position to beginning ot
  _currentPos = 0.0;
}

void CircleSource::update() {
  _lastFrame = ofGetFrameNum();

  // LOG_VIDEO_VERBOSE() << "updating";

  // update currentPos
  _currentPos = _currentPos + _speed;

  if(_currentPos >= 1.0) {
    // LOG_VIDEO_VERBOSE() << "pos >= 1: " << _currentPos;

    _currentPos = 0;
    createRandomPolyline(_path.getPointAtPercent(1));
  }

  // LOG_VIDEO_VERBOSE() << "updated pos: " << _currentPos;

}

// use x and y to translate origin
void CircleSource::draw(int x, int y) {
  if(!isFrameNew()) {
    return;
  }

  // LOG_VIDEO_VERBOSE() << "circle drawing: " << _path.getPointAtPercent(_currentPos);

  ofPushStyle();

  _fbo.begin();
  ofClear(_bkgColor);
  // ofBackground(_bkgColor);
  ofSetColor(_color);
  ofFill();
  // ofBackground(_color);
  ofPoint p = _path.getPointAtPercent(_currentPos);
  ofDrawCircle(p, _radius);
  // draw polyling for debug
  // ofSetColor(ofColor::cyan);
  // _path.draw();

  _fbo.end();

  _fbo.draw(x, y);

  ofPopStyle();

  // ofDrawCircle(_x, _y, _radius);

}

size_t CircleSource::getWidth() {
  return APP_WIDTH;
}

size_t CircleSource::getHeight() {
  return APP_HEIGHT;
}

bool CircleSource::isFrameNew() {
  return _lastFrame <= ofGetFrameNum();
}

ofPixels& CircleSource::getPixels() {
  _fbo.readToPixels(_pixels);
  return _pixels;
}


void CircleSource::createRandomPolyline(ofPoint start) {
  _path.clear();

  _path.addVertex(start);

  for(int i : std::views::iota(0, 10)) {
    ofPoint c1 = getRandomPoint();
    ofPoint c2 = getRandomPoint();
    ofPoint to = getRandomPoint();

    _path.bezierTo(c1, c2, to);
  }
}

ofPoint CircleSource::getRandomPoint() {
  return {ofRandom(0, getWidth()), ofRandom(0, getHeight())};
}

ofPoint CircleSource::getFocus() {
  ofPoint p = _path.getPointAtPercent(_currentPos);

  return {p.x / APP_WIDTH, p.y / APP_HEIGHT};
}

float CircleSource::getFocusRadius() {
  return static_cast<float>(_radius) / ofMin(APP_WIDTH, APP_HEIGHT);
}




//
// video
//
Video::Video() {}

Video::~Video() {
  ofRemoveListener(ofEvents().keyPressed, this, &Video::keyPressed);
}

void Video::setup() {
  // _source = CameraSource();

  // _source = FileSource();

  _source = CircleSource();

  std::visit([&](auto &&s) {
    s.setup();
  }, _source);

  _colorImg.allocate(_width, _height);
  _grayImage.allocate(_width, _height);
  _grayBg.allocate(_width, _height);
  _grayDiff.allocate(_width, _height);

  // _bLearnBakground = true;
  _threshold = 80;

  ofAddListener(ofEvents().keyPressed, this, &Video::keyPressed);

  ofPixels gray;

  visit([&gray](auto &&s) {
    gray.allocate(s.getWidth(), s.getHeight(), OF_IMAGE_GRAYSCALE);
  }, _source);
  gray.setColor(ofColor().wheat);

  _grayBg.setFromPixels(gray);
}

void Video::update() {
  visit([](auto &&s) {
    s.update();
  }, _source);

  if (!visit([](auto &&s){ return s.isFrameNew(); }, _source)) {
    return;
  }

  visit([&](auto &&s) {
    _colorImg.setFromPixels(s.getPixels());
  }, _source);

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
  std::visit([](auto &&s) {
    s.draw(0, 0);
  }, _source);
}

void Video::drawMetadata() {
  ofPushStyle();
  ofSetColor(ofColor::deepPink);

  _contourFinder.draw(0, 0);

  ofSetColor(ofColor::red);
  for (auto hull : polylineContours()) {
    hull.draw();
  }

  ofSetColor(ofColor::darkCyan);
  ofDrawBitmapString("threshold: " + ofToString(_threshold), 20, 20);
  ofDrawBitmapString("simplification: " + ofToString(_simplificationFactor), 20, 40);

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
    _threshold++;
    if (_threshold > 255) _threshold = 255;
    break;
  case '-':
    LOG_COMM_VERBOSE() << "threshold--";
    _threshold--;
    if (_threshold < 0) _threshold = 0;
    break;
  case 'w':
    LOG_VIDEO_VERBOSE() << "_simplificationFactor++";
    if (_simplificationFactor > 10) {
      _simplificationFactor += 10;
    } else if(_simplificationFactor > 1){
      _simplificationFactor += 1;
    } else {
      _simplificationFactor += 0.1;
    }
    break;
  case 'q':
    LOG_VIDEO_VERBOSE() << "_simplificationFactor--";
    if (_simplificationFactor >= 20) {
      _simplificationFactor -= 10;
    } else if(_simplificationFactor > 1){
      _simplificationFactor -= 1;
    } else {
      _simplificationFactor -= 0.1;
    }

    if (_simplificationFactor <= 0.0) {
      _simplificationFactor = 0.1;
    }
    break;

  }
}

//
// get found contours
//
vector<ofxCvBlob> &Video::contours() {
  return _contourFinder.blobs;
}

void simplifyContour (ofPolyline &points, float factor) {
  float f = factor;

  while (points.size() >= CONTOUR_MAX_POINTS) {
    // LOG_COMM_WARNING() << "too many points in contour: " << points.size() << " approximating it";
    int s = points.size();

    points.simplify(f);

    if (points.size() == s) {
      f *= 2;
    }
  }
}

vector<ofPolyline> Video::polylineContours() {
  vector<ofPolyline> r;

  for (auto contour : contours()) {
    ofPolyline p;

    for (auto pt : contour.pts) {
      p.addVertex({pt.x, pt.y, 0});
    }
    p.setClosed(true);
    simplifyContour(p, _simplificationFactor);

    r.push_back(p);
  }

  return r;
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

//
// get source-depending focus position
//
ofPoint Video::getFocus() {
  return std::visit([](auto &&s){
    return s.getFocus();
  },
    _source);
}

//
// get source-depending focus radius
//
float Video::getFocusRadius() {
  return std::visit([](auto &&s){
    return s.getFocusRadius();
  },
    _source);
}
