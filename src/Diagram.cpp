#include "Diagram.h"
#include "Magick++/Color.h"
#include "MagickCore/image.h"
#include "ofAppGlutWindow.h"
#include "ofEventUtils.h"
#include "ofEvents.h"
#include "ofGraphics.h"
#include "ofGraphicsConstants.h"
#include "ofMain.h"
#include "ofShader.h"
#include "ofThread.h"
#include <optional>


//
// diagram loader
//

DiagramLoader::DiagramLoader() {};

void DiagramLoader::load(string path) {
  if (_loading) {
    LOG_DIAGRAM_WARNING() << "diagram is being loaded. ignoring";
    return;
  }

  if (_loaded) {
    LOG_DIAGRAM_WARNING() << "diagram is already loaded but not yet fetched. ignoring loading new one";
    return;
  }

  _path = path;
  _loaded = false;
  _loading = true;
  startThread();
}

void DiagramLoader::doLoad() {
  Magick::Image my_image{};

  // Magick::Color c{0x00, 0x00, 0x00, 0xFFFF};
  my_image.backgroundColor("None");
  // my_image.debug();
  // my_image.depth(16);
  // stringstream p;
  // p << "svgz:" << ofToDataPath(path);

  my_image.read(ofToDataPath(_path));
  my_image.type(MagickCore::ImageType::TrueColorAlphaType);

  // my_image.verbose();
  // my_image.depth(16);
  // my_image.write(ofToDataPath("diagrams/test_out.png"));
  Magick::Geometry bbox = my_image.boundingBox();

  // ofLog(OF_LOG_NOTICE) << "svg format: " << my_image.format();
  // ofLog(OF_LOG_NOTICE) << "svg width: " << bbox.width();
  // ofLog(OF_LOG_NOTICE) << "svg height: " << bbox.height();

  _diagramPix.setImageType(ofImageType::OF_IMAGE_COLOR_ALPHA);
  _diagramPix.allocate(bbox.width(), bbox.height(), 4);

  for (unsigned int j = 0; j < bbox.height(); j++) {
    for (unsigned int i = 0; i < bbox.width(); i++) {

      Magick::ColorRGB c = my_image.pixelColor(i, j);

      _diagramPix.setColor(i, j, ofFloatColor(c.red(), c.green(), c.blue(), c.alpha()));
      // _diagramPix.setColor(i,
      //   j,
      //   ofFloatColor(toOfPixelColor(c.redQuantum()),
      //     toOfPixelColor(c.greenQuantum()),
      //     toOfPixelColor(c.blueQuantum())));
    }
  }
}

void DiagramLoader::threadedFunction(){
  doLoad();
  LOG_DIAGRAM_NOTICE() << "diagram was loaded";
  _loading = false;
  _loaded = true;
}

// can be called only once
std::optional<ofFloatPixels> DiagramLoader::tryGetDiagram() {
  if (_loading) {
    return std::nullopt;
  }

  if (!_loaded) {
    return std::nullopt;
  }

  // flushing
  _loaded = false;
  _loading = false;

  return _diagramPix;
}

//
// diagram class defs
//
Diagram::Diagram() {
}

Diagram::~Diagram() {
  ofRemoveListener(ofEvents().windowResized, this, &Diagram::onWindowResize);
}

void Diagram::setup() {
  ofAddListener(ofEvents().windowResized, this, &Diagram::onWindowResize);
  ofAddListener(ofEvents().keyReleased, this, &Diagram::onKeyPressed);
  loadDiagram();
}

void Diagram::update() {
  // loadDiagram(DIAGRAM_FILE);

  // update diagram is there is one
  auto d = _loader.tryGetDiagram();
  if (d.has_value()) {
    LOG_DIAGRAM_NOTICE() << "using async-loaded diagram";

    _diagramImage.setFromPixels(d.value());

    recalculateScales();

    int r = 1;
    ofNotifyEvent(onUpdated, r, this);

  }

}

// FIXME: pass x y as args instead defaulting them to 0
void Diagram::draw () {
  if (!_diagramImage.isAllocated()) {
    return;
  }

  if (!_drawable) {
    return;
  }

  ofPushStyle();
  ofPushView();
  scale();
  _diagramImage.draw(0, 0);
  ofPopStyle();
  ofPopView();
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

void Diagram::loadDiagram() {
  _loader.load(DIAGRAM_FILE);
}


void Diagram::recalculateScales() {
  if (!_diagramImage.isAllocated()) {
    _scaleX = 1;
    _scaleY = 1;
    return;
  }
  _scaleX = static_cast<float>(_diagramImage.getWidth()) / ofGetWindowWidth();
  _scaleY = static_cast<float>(_diagramImage.getHeight()) / ofGetWindowHeight();

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

//
// on window resize
//
void Diagram::onKeyPressed(ofKeyEventArgs &args) {
  if (args.key == 'd') {
    LOG_DIAGRAM_NOTICE() << "toggling diagram";
    _drawable = !_drawable;
  }
}
