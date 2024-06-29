#include "Diagram.h"
#include "Magick++/Color.h"
#include "ofEventUtils.h"
#include "ofGraphics.h"
#include "ofMain.h"
#include "ofShader.h"

Diagram::Diagram() {
}

Diagram::~Diagram() {
  ofRemoveListener(ofEvents().windowResized, this, &Diagram::onWindowResize);
}

void Diagram::setup() {
  ofAddListener(ofEvents().windowResized, this, &Diagram::onWindowResize);
  loadDiagram("diagrams/map_patched.svg");
}

void Diagram::update() {
}

// FIXME: pass x y as args instead defaulting them to 0
void Diagram::draw () {
  ofPushStyle();
  scale();
  _diagramImage.draw(0, 0);
  ofPopStyle();
}

//
//
// load diagram
//
//
// float toOfPixelColor (Magick::Quantum c) {
//   return static_cast<float>(Magick::Color::scaleQuantumToDouble(c));
//   //return ScaleQuantumToChar(c);

//   // float cF = static_cast<float>(c) / static_cast<float>(std::numeric_limits<Magick::Quantum>::max());

//   // Magick::QuantumType a;
//   // return static_cast<unsigned char>(cF * ofColor::limit());
// }

void Diagram::loadDiagram(string path) {
  Magick::Image my_image{};

  Magick::Color c{0x00, 0x00, 0x00, 0xFFFF};
  my_image.backgroundColor(c);
  // my_image.debug();
  // my_image.depth(16);
  // stringstream p;
  // p << "svgz:" << ofToDataPath(path);

  my_image.read(ofToDataPath(path));
  // my_image.verbose();
  // my_image.depth(16);
  // my_image.write(ofToDataPath("diagrams/test_out.png"));
  Magick::Geometry bbox = my_image.boundingBox();

  // ofLog(OF_LOG_NOTICE) << "svg format: " << my_image.format();
  // ofLog(OF_LOG_NOTICE) << "svg width: " << bbox.width();
  // ofLog(OF_LOG_NOTICE) << "svg height: " << bbox.height();

  _diagramPix.allocate(bbox.width(), bbox.height(), 3);

  for (unsigned int j = 0; j < bbox.height(); j++) {
    for (unsigned int i = 0; i < bbox.width(); i++) {

      Magick::ColorRGB c = my_image.pixelColor(i, j);

      _diagramPix.setColor(i, j, ofFloatColor(c.red(), c.green(), c.blue()));
      // _diagramPix.setColor(i,
      //   j,
      //   ofFloatColor(toOfPixelColor(c.redQuantum()),
      //     toOfPixelColor(c.greenQuantum()),
      //     toOfPixelColor(c.blueQuantum())));
    }
  }

  _diagramImage.setFromPixels(_diagramPix);

  recalculateScales();
}


void Diagram::recalculateScales() {
  _scaleX = static_cast<float>(_diagramPix.getWidth()) / ofGetWindowWidth();
  _scaleY = static_cast<float>(_diagramPix.getHeight()) / ofGetWindowHeight();

}

//
//
// scaling to window size
//
//
void Diagram::scale() {
  if (_scaleX > _scaleY) {
    ofScale(1.0 / _scaleX, 1.0 / _scaleX, 1);
  } else {
    ofScale(1.0 / _scaleY, 1.0 / _scaleY, 1);
  }
}


//
// on window resize
//
void Diagram::onWindowResize(ofResizeEventArgs &args) {
  recalculateScales();
}
