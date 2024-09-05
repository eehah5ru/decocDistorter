#pragma once

#include "Magick++/Color.h"
#include "Magick++/Geometry.h"
#include "Magick++/Image.h"
#include "Magick++/Include.h"
#include "Magick++/Pixels.h"
#include "MagickCore/magick-type.h"
#include "MagickCore/quantum.h"
#include "Recorder.hpp"
#include "fwd.hpp"
#include "ofAppGlutWindow.h"
#include "ofFileUtils.h"
#include "ofGraphicsConstants.h"
#include "ofMain.h"
#include "ofAppRunner.h"
#include "ofColor.h"
#include "ofEventUtils.h"
#include "ofEvents.h"
#include "ofGraphics.h"
#include "ofImage.h"
#include "ofThreadChannel.h"
#include <cstddef>
#include <cstdint>
#include <format>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>

#define LOG_RECORDER_ERROR() ofLogError( "RECORDER" ) << __FUNCTION__ << ": "
#define LOG_RECORDER_WARNING() ofLogWarning( "RECORDER" ) << __FUNCTION__ << ": "
#define LOG_RECORDER_NOTICE() ofLogNotice( "RECORDER" ) << __FUNCTION__ << ": "
#define LOG_RECORDER_VERBOSE() ofLogVerbose( "RECORDER" ) << __FUNCTION__ << ": "
#define LOG_RECORDER() LOG_RECORDER_NOTICE()

using namespace std;

template <class ImageClass>
class Recorder {

  struct Frame {
    uint64_t frameNum;
    shared_ptr<ImageClass> frame;
  };

  bool _isRecording = false;
  string _path = "";

  ofThreadChannel<Frame> _saverChannel;

  unique_ptr<jthread> _saver;

public:

  Recorder () {
  }

  void update() {
  }

  void setup(string filePattern) {
    _path = filePattern;

    ofAddListener(ofEvents().keyReleased, this, &Recorder::onKeyPressed);

    // creating and starting;
    _saver = make_unique<jthread>(jthread{&Recorder::doSaveFrame, this});
  }

  void exit() {
    _saverChannel.close();
  }

  void draw(int x, int y) {
    if (!_isRecording && _saverChannel.empty()) {
      return;
    }

    ofPushStyle();
    ofSetColor(ofColor::red);
    // right top corner
    ofDrawCircle({x, y}, 30);
    ofPopStyle();
  }

  void recordFrame() {
    if(!_isRecording) {
      return;
    }

    if (_path.empty()) {
      throw invalid_argument("path must be initialised");
    }

    shared_ptr<ImageClass> screen = make_shared<ImageClass>(ImageClass());
    screen->grabScreen(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    auto frameNum = ofGetFrameNum();

    _saverChannel.send(Frame{frameNum, screen});
  }

  void recordFrame(shared_ptr<ImageClass> image) {
    // LOG_RECORDER_VERBOSE() << "1";

    if(!_isRecording) {
      return;
    }

    if (_path.empty()) {
      throw invalid_argument("path must be initialised");
    }

    // LOG_RECORDER_VERBOSE() << "recording frame from image";

    // ImageClass screen;
    // screen.setImageType(ofImageType::OF_IMAGE_COLOR_ALPHA);
    // screen.setImageType(image.getImageType());
    // screen.setFromPixels(image.getPixels());

    // ofFloatPixels p;
    // p.setImageType(image.getImageType());


    auto frameNum = ofGetFrameNum();

    // image.get()->save(ofToDataPath("testframe.png"));
    _saverChannel.send(Frame{frameNum, image});
  }


  // void recordFrameFromPixels (ofPixels &&pixels) {
  //   LOG_RECORDER_VERBOSE() << "try to record from pixels";

  //   if(!_isRecording) {
  //     return;
  //   }

  //   if (_path.empty()) {
  //     throw invalid_argument("path must be initialised");
  //   }

  //   LOG_RECORDER_VERBOSE() << "recording frame from pixels";

  //   ofImage &&screen = ofImage();
  //   screen.setFromPixels(pixels);
  //   auto frameNum = ofGetFrameNum();

  //   _saverChannel.send(Frame{frameNum, screen});

  // }



  // void doSaveFrame () {
  //   Frame frame;
  //   while(_saverChannel.receive(frame)) {
  //     string path = vformat(_path, make_format_args(frame.frameNum));

  //     // frame.frame->setImageType(OF_IMAGE_COLOR);
  //     frame.frame->save(path);
  //   }
  // }

  void doSaveFrame () {
    Frame frame;
    while(_saverChannel.receive(frame)) {
      string path = vformat(_path, make_format_args(frame.frameNum));

      Magick::Image out{Magick::Geometry(frame.frame->getWidth(), frame.frame->getHeight()), "cyan"};
      // out.resize(Magick::Geometry(frame.frame->getWidth(), frame.frame->getHeight()));
      out.type(Magick::TrueColorAlphaType);
      out.backgroundColor("None");
      out.magick("PNG");

      out.modifyImage();
      Magick::Pixels view(out);

      // LOG_RECORDER_VERBOSE() << "out channels: " << out.channels();
      // LOG_RECORDER_VERBOSE() << "out geometry: " << out.rows() << " " << out.columns();
      // LOG_RECORDER_VERBOSE() << "quantum depth: " << MAGICKCORE_QUANTUM_DEPTH;
      // LOG_RECORDER_VERBOSE() << "quantum range: " << ;


      MagickCore::Quantum *out_pxs = view.get(0, 0, out.columns(), out.rows());

      // span<MagickCore::Quantum> out_pxs_span = span<MagickCore::Quantum>(out_pxs, out.columns() * out.rows() * out.channels() * out.channels());


      // MagickCore::Quantum *out_pxs = view.get(0, 0, 10, 10);

      ofFloatPixels pxs = frame.frame->getPixels();

      // std::size_t i = 0;

      for (std::size_t row = 0; row < out.rows(); row++) {
        for (std::size_t column = 0; column < out.columns() ; column++) {
          ofFloatColor inPx = pxs.getColor(out.columns() - column, out.rows() - row);


          out_pxs[out.columns() * out.channels() * (out.rows() - row - 1) + (out.columns() - column - 1) * out.channels() + 0] = inPx.r * 65535.0;
          out_pxs[out.columns() * out.channels() * (out.rows() - row - 1) + (out.columns() - column - 1) * out.channels() + 1] = inPx.g * 65535.0;
          out_pxs[out.columns() * out.channels() * (out.rows() - row - 1) + (out.columns() - column - 1) * out.channels() + 2] = inPx.b * 65535.0;
          out_pxs[out.columns() * out.channels() * (out.rows() - row - 1) + (out.columns() - column - 1) * out.channels() + 3] = inPx.a * 65535.0;
        }

      }
      // for (size_t i : std::ranges::views::iota(10)) {
      //   for (size_t j : std::views::iota(10)) {
      //     ofColor inPx = pxs.getColor(i, j);
      //     MagickCore::Quantum *out_px = out_pxs + i*j + j;
      //     out_px[0] = inPx.r;
      //     out_px[1] = inPx.g;
      //     out_px[2] = inPx.b;
      //     // out_px[3] = inPx.a;
      //   }
      // }
      view.sync();
      // out.syncPixels();


      out.write(ofToDataPath(path, true));
    }
  }

  void onKeyPressed(ofKeyEventArgs &key) {
    if (key.key == 'r') {
      _isRecording = !_isRecording;
    }
  }
};
