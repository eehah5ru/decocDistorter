#include "Communicator.h"
#include "ofAppRunner.h"
#include "ofMathConstants.h"
#include "ofxCvBlob.h"
#include "ofxOscMessage.h"
#include "ofxOscSender.h"
#include <vector>
#include <memory>

namespace comm {
  //
  //
  // COMMUNICATOR
  //
  //
  Communicator::Communicator()
    : _sender{_senderOut} {
  }

  Communicator::~Communicator() {
  }

  //
  // setup
  //
  void Communicator::setup() {
    _sender.setup();

    _sender.startThread();
  }

  //
  // EXIT
  //
  void Communicator::exit() {
    _sender.stopThread();
  }

  //
  //
  // SENDS
  //
  //

  //
  // send contours
  //
  void Communicator::sendContours(vector<ofxCvBlob> contours) {
    std::shared_ptr<SenderInData> cmd = std::make_shared<SenderCountoursCmd>(contours);


    // _senderOut.send(cmd);

  }

  //
  // send mouse position
  //
  void Communicator::sendMouse(int x, int y) {
    // normalized to window size
    float f_x = ofMap(x, 0, ofGetWidth(), 0.f, 1.f, true);
    float f_y = ofMap(y, 0, ofGetHeight(), 0.f, 1.f, true);

    std::shared_ptr<SenderInData> cmd = std::make_shared<SenderMouseCmd>(f_x, f_y);

    // SenderMouseCmd cmd{f_x, f_y};
    _senderOut.send(cmd);
  }

  //
  // send shake poositions
  //
  void Communicator::sendShakePositions() {
    auto cmd = std::make_shared<SenderShakePositionsCmd>();

    _senderOut.send(cmd);
  }

  //
  //
  // SENDER
  //
  //

  Sender::Sender(SenderInChannel &in)
    : _inChannel{in} {
  }

  Sender::~Sender() {
  }

  void Sender::setup() {
    _oscSender.setup(_remoteAddress, _remotePort);
  }


  void Sender::threadedFunction() {
    while (isThreadRunning()) {
      // auto cmd = make_shared<SenderInData>();
      auto cmd = _inChannel.receive();

      // _inChannel.receive(cmd);

      if (cmd.has_value()) {
        cmd.value()->send(_oscSender);
      }
    }
  }

  //
  //
  // SENDER CMDS
  //
  //

  // SenderInData::SenderInData() {}

  void SenderInData::send(ofxOscSender &oscSender) {
    throw std::logic_error("SenderInData.send must be overriden");
  }

  //
  //
  // SENDER CMD / CONTOURS
  //
  //
  SenderCountoursCmd::SenderCountoursCmd(vector<ofxCvBlob> contours)
    : _contours{contours} {
  }

  // SenderCountoursCmd::~SenderCountoursCmd() {}

  void SenderCountoursCmd::send(ofxOscSender &oscSender) {
    send_sendingContours(oscSender, _contours.size());

      for (ofxCvBlob contour : _contours) {
        send_countour(oscSender, contour);
      }
  }

  void SenderCountoursCmd::send_sendingContours(ofxOscSender oscSender, int numContours) {
    ofxOscMessage msg;

    msg.setAddress("/contours");
    msg.addInt32Arg(numContours);

    oscSender.sendMessage(msg, false);
  }

  void SenderCountoursCmd::send_countour(ofxOscSender oscSender, ofxCvBlob &contour) {
    ofxOscMessage msg;

    msg.setAddress("/contour");

    msg.addInt32Arg(contour.nPts);

    for (ofDefaultVec3 point : contour.pts) {
      msg.addFloatArg(point.x);
      msg.addFloatArg(point.y);
    }

    oscSender.sendMessage(msg, false);
  }


  //
  //
  // SENDER CMD / MOUSE
  //
  //

  SenderMouseCmd::SenderMouseCmd(float x, float y)
    : _x{x}
    , _y{y} {}

  // SenderMouseCmd::~SenderMouseCmd() {}

  void SenderMouseCmd::send(ofxOscSender &oscSender) {
    // std::cerr << "SenderMouseCmd.send" << endl;

    ofxOscMessage m;
    m.setAddress("/mouse/position");
    m.addFloatArg(_x);
    m.addFloatArg(_y);
    oscSender.sendMessage(m, false);
  }


  //
  //
  // SENDER CMD / SHAKE POSITIONS
  //
  //
  void SenderShakePositionsCmd::send(ofxOscSender &oscSender) {
    ofxOscMessage m;
    m.setAddress("/shake/positions");
    oscSender.sendMessage(m, false);
  }

}
