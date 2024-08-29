#include <ranges>

#include "Communicator.h"
#include "ofAppRunner.h"
#include "ofEventUtils.h"
#include "ofMathConstants.h"
#include "ofPolyline.h"
#include "ofxCvBlob.h"
#include "ofxOscMessage.h"
#include "ofxOscSender.h"
#include <vector>
#include <memory>


namespace comm {

  //
  //
  // UTILS
  //
  //

  //
  // approximate contour till it fits in CONTOUR_MAX_POINTS
  //

  ofPolyline approximateContour (ofPolyline points) {
    if (points.size() <= CONTOUR_MAX_POINTS) {
      return points;
    }

    LOG_COMM_WARNING() << "too many points in contour: " << points.size() << " approximating it";

    // points.simplify(0.3);
    vector<ofDefaultVec3> r;

    int i = 0;
    for (auto &p : points) {
      if (0 == i++ % 2) {
        r.push_back(p);
      }
    }
    return approximateContour(r);
  }

  void simplifyContour (ofPolyline &points) {


    while (points.size() >= CONTOUR_MAX_POINTS) {
      // LOG_COMM_WARNING() << "too many points in contour: " << points.size() << " approximating it";

      points.simplify(100.0f);
    }
  }

  //
  //
  // COMMUNICATOR
  //
  //
  Communicator::Communicator()
    : _sender{_senderOut}
    , onMapUpdated{} {
  }

  Communicator::~Communicator() {
  }

  //
  // setup
  //
  void Communicator::setup() {
    _sender.setup();
    // FIXME: unhardcode
    _receiver.setup(12346);

    _sender.startThread();
  }

  //
  // UPDATE
  //
  void Communicator::update () {
    while (_receiver.hasWaitingMessages()) {
      ofxOscMessage m;
      _receiver.getNextMessage(m);

      int r = 1;
      ofNotifyEvent(onMapUpdated, r, this);
    }
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
    vector<ofPolyline> r;

    for (auto contour : contours) {
      ofPolyline p;

      for (auto point :contour.pts) {
        p.addVertex({point.x, point.y, 0});
      }

      r.push_back(p);
    }

    sendContours(r);
  }

  void Communicator::sendContours(vector<ofPolyline> contours) {
    std::shared_ptr<SenderInData> cmd = std::make_shared<SenderCountoursCmd>(contours);

    _senderOut.send(cmd);

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
  SenderCountoursCmd::SenderCountoursCmd(vector<ofPolyline> contours)
    : _contours{contours} {
  }

  // SenderCountoursCmd::~SenderCountoursCmd() {}

  void SenderCountoursCmd::send(ofxOscSender &oscSender) {
    send_sendingContours(oscSender, _contours.size());

      for (ofPolyline contour : _contours) {
        send_countour(oscSender, contour);
      }
  }

  void SenderCountoursCmd::send_sendingContours(ofxOscSender oscSender, int numContours) {
    ofxOscMessage msg;

    // LOG_COMM() << "send " << numContours << " countours";

    msg.setAddress("/contours");

    msg.addInt32Arg(numContours);

    oscSender.sendMessage(msg, false);
  }

  void SenderCountoursCmd::send_countour(ofxOscSender oscSender, ofPolyline &contour) {
    ofxOscMessage msg;

    LOG_COMM() << "sending countour with " << contour.size() << " points";

    // simplifyContour(contour);

    // auto points = approximateContour(contour);

    // auto points = contour.pts;

    msg.setAddress("/contour");

    msg.addInt32Arg(contour.size());

    for (ofDefaultVec3 point : contour) {
      float f_x = ofMap(point.x, 0, ofGetWindowWidth(), 0.f, 1.f, true);
      float f_y = ofMap(point.y, 0, ofGetWindowHeight(), 0.f, 1.f, true);
      msg.addFloatArg(f_x);
      msg.addFloatArg(f_y);
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
