#pragma once

#include "ofEvents.h"
#include "ofMain.h"
#include "ofVideoGrabber.h"
#include "ofxCvBlob.h"
#include "ofxOpenCv.h"

class Video {
  ofVideoGrabber _grabber;
  ofxCvColorImage _colorImg;
  ofxCvGrayscaleImage   _grayImage;
  ofxCvGrayscaleImage   _grayBg;
  ofxCvGrayscaleImage   _grayDiff;

  ofxCvContourFinder  _contourFinder;

  int         _threshold = 50;
  bool				_bLearnBakground = false;

  const int _width = 320;
  const int _height = 240;

  void debugContours();
 public:

  Video();
  ~Video();

  void setup();

  void update ();

  void draw();

  void keyPressed (ofKeyEventArgs &key);


  vector<ofxCvBlob> &contours();
};
