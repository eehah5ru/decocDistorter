#pragma once

#include "ofEvents.h"
#include "ofMain.h"
#include "ofPolyline.h"
#include "ofVideoGrabber.h"
#include "ofxCvBlob.h"
#include "ofxOpenCv.h"
#include "ofxConvexHull.h"

#define LOG_VIDEO_ERROR() ofLogError( "VIDEO" ) << __FUNCTION__ << ": "
#define LOG_VIDEO_WARNING() ofLogWarning( "VIDEO" ) << __FUNCTION__ << ": "
#define LOG_VIDEO_NOTICE() ofLogNotice( "VIDEO" ) << __FUNCTION__ << ": "
#define LOG_VIDEO_VERBOSE() ofLogVerbose( "VIDEO" ) << __FUNCTION__ << ": "
#define LOG_VIDEO() LOG_VIDEO_NOTICE()

class Video {
  ofVideoGrabber _grabber;
  ofxCvColorImage _colorImg;
  ofxCvGrayscaleImage   _grayImage;
  ofxCvGrayscaleImage   _grayBg;
  ofxCvGrayscaleImage   _grayDiff;

  ofxCvContourFinder  _contourFinder;
  ofxConvexHull _convexHull;

  int         _threshold = 50;
  float _simplificationFactor = 50;

  bool				_bLearnBakground = false;

  const int _width = 1280;
  const int _height = 720;

  void debugContours();
 public:

  Video();
  ~Video();

  void setup();

  void update ();

  void drawAll();
  void drawVideo();
  void drawMetadata();


  void keyPressed (ofKeyEventArgs &key);


  vector<ofxCvBlob> &contours();

  vector<ofPolyline> polylineContours();

  vector<ofPolyline> convexHulls();

  vector<ofPolyline> boundingBoxes();
};
