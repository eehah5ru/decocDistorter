#pragma once

#include "ofAppGlutWindow.h"
#include "ofEvents.h"
#include "ofGraphicsBaseTypes.h"
#include "ofMain.h"
#include "settings.h"
#include <Magick++.h>
#include <memory>
#include <optional>

using namespace std;

#define LOG_DIAGRAM_ERROR() ofLogError( "DIAGRAM" ) << __FUNCTION__ << ": "
#define LOG_DIAGRAM_WARNING() ofLogWarning( "DIAGRAM" ) << __FUNCTION__ << ": "
#define LOG_DIAGRAM_NOTICE() ofLogNotice( "DIAGRAM" ) << __FUNCTION__ << ": "
#define LOG_DIAGRAM_VERBOSE() ofLogVerbose( "DIAGRAM" ) << __FUNCTION__ << ": "
#define LOG_DIAGRAM() LOG_DIAGRAM_NOTICE()

class DiagramLoader: public ofThread {
  ofFloatPixels _diagramPix;

  string _path = "";

  bool _loaded = false;
  bool _loading = false;

  bool _isAsync;

  void doLoad();


  void threadedFunction();

  void loadSync(string path);

  void loadAsync(string path);

public:
  DiagramLoader(bool isAsync = APP_ASYNC_MODE);

  void load(string path);

  std::optional<ofFloatPixels> tryGetDiagram();
};

class Diagram {
  const string DIAGRAM_FILE = "diagrams/map_patched.svg";

  ofFloatImage _diagramImage;

  // async loader == true
  DiagramLoader _loader = {APP_ASYNC_MODE};

  float _scaleX = 1.0;
  float _scaleY = 1.0;

  bool _drawable = true;

  void recalculateScales();

 public:
  ofEvent<int> onUpdated;

  Diagram();
  ~Diagram();

  void setup();
  void update();
  void draw();

  void scale();

  void loadDiagram();

  shared_ptr<ofFloatImage> getImage();

  void onWindowResize(ofResizeEventArgs &args);
  void onKeyPressed(ofKeyEventArgs &args);
};
