#pragma once

#include "ofEvent.h"
#include "ofEvents.h"
#include "ofMain.h"
#include "ofPoint.h"
#include "ofPolyline.h"
#include "ofThread.h"
#include "ofThreadChannel.h"
#include "ofxCvBlob.h"
#include "ofxOsc.h"
#include "ofxOscSender.h"
#include <memory>
#include "myThreadChannel.hpp"
#include <ranges>

#define LOG_COMM_ERROR() ofLogError( "Communicator" ) << __FUNCTION__ << ": "
#define LOG_COMM_WARNING() ofLogWarning( "Communicator" ) << __FUNCTION__ << ": "
#define LOG_COMM_NOTICE() ofLogNotice( "Communicator" ) << __FUNCTION__ << ": "
#define LOG_COMM_VERBOSE() ofLogVerbose( "Communicator" ) << __FUNCTION__ << ": "
#define LOG_COMM() LOG_COMM_NOTICE()

#define CONTOUR_MAX_POINTS 20

namespace comm {
  // using SenderInData = vector<ofxCvBlob>;


  // class SenderInCmd {
  //   vector<ofxCvBlob> _contours;

  // };

  //
  // Sender Cmd interface
  //
  class SenderInData {
  public:
    virtual ~SenderInData() = default;
    SenderInData(const SenderInData&) = default;
    SenderInData& operator=(const SenderInData&) = default;
    SenderInData(SenderInData&&) = default;
    SenderInData& operator=(SenderInData&&) = default;

    SenderInData() = default;

    // SenderInData(const SenderInData&) = delete;
    // SenderInData& operator=(const SenderInData&) = delete;

    friend void swap(SenderInData& a, SenderInData& b) noexcept {
        using std::swap;
    }

    void virtual send(ofxOscSender &oscSender);
  };

  // using ChannelData = std::unique_ptr<SenderInData>;
  using ChannelData = SenderInData;
  using SenderInChannel = myThreadChannel<SenderInData>;



  //
  // Send contours
  //
  class SenderCountoursCmd : public SenderInData{
    vector<ofPolyline> _contours;

    void send_sendingContours(ofxOscSender oscSender, int numContours);

    void send_countour(ofxOscSender oscSender, ofPolyline &contour);

  public:
    SenderCountoursCmd(vector<ofPolyline> contours);
    // ~SenderCountoursCmd();

    friend void swap(SenderCountoursCmd& a, SenderCountoursCmd& b) noexcept
    {
        using std::swap;
        swap(static_cast<SenderInData&>(a), static_cast<SenderInData&>(b));
        swap(a._contours, b._contours);
    }

    void virtual send(ofxOscSender &oscSender) override;
  };

  //
  // Send Mouse
  //
  class SenderMouseCmd : public SenderInData {
    float _x;
    float _y;
  public:
    SenderMouseCmd(float x, float y);
    // ~SenderMouseCmd();

    void virtual send(ofxOscSender &oscSender) override;

  };

  //
  // Send Mouse
  //
  class SenderFocusCmd : public SenderInData {
    ofPoint _focus;
    float _radius;

  public:
    SenderFocusCmd(ofPoint focus, float radius);

    void virtual send(ofxOscSender &oscSender) override;

  };


  //
  // Shake positions in contours CMD
  //
  class SenderShakePositionsInContoursCmd : public SenderInData {
  public:
    void virtual send(ofxOscSender &oscSender) override;
  };

  //
  // Shake positions near focusCMD
  //
  class SenderShakePositionsNearFocusCmd : public SenderInData {
  public:
    void virtual send(ofxOscSender &oscSender) override;
  };



  //
  // threads
  //
  class Sender : public ofThread {
    SenderInChannel &_inChannel;

    ofxOscSender _oscSender;

    string _remoteAddress = "127.0.0.1"; // IP address we're sending to
    int _remotePort = 12345; // port we're sending to

  public:
    Sender(SenderInChannel &in);
    ~Sender();

    void setup();

    void threadedFunction() override;
  };

  // class Receiver : public ofThread {
  // };

  //
  // messages
  //


  //
  // comm class
  //
  class Communicator {
    myThreadChannel<SenderInData> _senderOut;
    // ofThreadChannel<>

    Sender _sender;

    ofxOscReceiver _receiver;


  public:

    ofEvent<int> onMapUpdated;

    Communicator();
    ~Communicator();

    void setup();

    void updateAsync ();

    void waitUnillNextIncomingMessage();

    void exit();

    void sendContours(vector<ofxCvBlob> contours);

    void sendContours(vector<ofPolyline> contours);


    void sendMouse(int x, int y);
    void sendFocus(ofPoint, float radius);


    void sendShakePositionsNearFocus();
    void sendShakePositionsInContours();

  };


  //
  //
  // UTILS
  //
  //

  // auto approximateContour (std::ranges::range auto& points);
}
