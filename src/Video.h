#pragma once

#include "settings.h"
#include "ofEvents.h"
#include "ofMain.h"
#include "ofPixels.h"
#include "ofPolyline.h"
#include "ofVideoGrabber.h"
#include "ofVideoPlayer.h"
#include "ofxCvBlob.h"
#include "ofxOpenCv.h"
#include "ofxConvexHull.h"
#include <cstddef>
#include <memory>
#include <variant>

#define LOG_VIDEO_ERROR() ofLogError( "VIDEO" ) << __FUNCTION__ << ": "
#define LOG_VIDEO_WARNING() ofLogWarning( "VIDEO" ) << __FUNCTION__ << ": "
#define LOG_VIDEO_NOTICE() ofLogNotice( "VIDEO" ) << __FUNCTION__ << ": "
#define LOG_VIDEO_VERBOSE() ofLogVerbose( "VIDEO" ) << __FUNCTION__ << ": "
#define LOG_VIDEO() LOG_VIDEO_NOTICE()



// class VideoSource{
// public:

//   // virtual ~VideoSource() = ;

//   virtual void setup() = 0;
//   virtual void update() = 0;
//   virtual void draw(int x, int y) = 0;
//   virtual size_t getWidth() = 0;
//   virtual size_t getHeight() = 0;
//   virtual bool isFrameNew() = 0;
//   virtual ofPixels& getPixels() = 0;

// };


class CameraSource {
  ofVideoGrabber _grabber;

  size_t _width = APP_WIDTH;
  size_t _height = APP_HEIGHT;
public:

  CameraSource();
  ~CameraSource();

  void setup();
  void update();
  void draw(int x, int y);
  size_t getWidth();
  size_t getHeight();
  bool isFrameNew();
  ofPixels& getPixels();
};

class FileSource {
  ofVideoPlayer _player;

  size_t _width = APP_WIDTH;
  size_t _height = APP_HEIGHT;
public:

  // CameraSource(int width, int height);

  void setup();
  void update();
  void draw(int x, int y);
  size_t getWidth();
  size_t getHeight();
  bool isFrameNew();
  ofPixels& getPixels();
};


class Video {
  int _width = APP_WIDTH;
  int _height = APP_HEIGHT;

  std::variant<CameraSource, FileSource> _source;

  // std::unique_ptr<VideoSource> _source = std::make_unique<CameraSource>(nullptr);

  ofxCvColorImage _colorImg;
  ofxCvGrayscaleImage   _grayImage;
  ofxCvGrayscaleImage   _grayBg;
  ofxCvGrayscaleImage   _grayDiff;

  ofxCvContourFinder  _contourFinder;
  ofxConvexHull _convexHull;

  int         _threshold = 50;
  float _simplificationFactor = 50;

  bool				_bLearnBakground = false;

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
