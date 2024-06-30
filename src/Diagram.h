#pragma once

#include "ofEvents.h"
#include "ofMain.h"
#include <Magick++.h>

using namespace std;

class Diagram {
  const string DIAGRAM_FILE = "diagrams/map_patched.svg";

  ofFloatPixels _diagramPix;
  ofFloatImage _diagramImage;

  float _scaleX = 1.0;
  float _scaleY = 1.0;


  void loadDiagram(string path);

  void recalculateScales();

 public:

  Diagram();
  ~Diagram();

  void setup();
  void update();
  void draw();

  void scale();


  void onWindowResize(ofResizeEventArgs &args);
};
