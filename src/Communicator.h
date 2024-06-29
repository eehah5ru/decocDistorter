#pragma once

#include "ofMain.h"
#include "ofThread.h"
#include "ofThreadChannel.h"
#include "ofxCvBlob.h"
#include "ofxOsc.h"
#include "ofxOscSender.h"
#include <memory>

namespace comm {
  // using SenderInData = vector<ofxCvBlob>;


  //
  // Sender Cmd interface
  //
  class SenderInData {
  public:
    SenderInData();

    SenderInData(const SenderInData&) = delete;
    SenderInData& operator=(const SenderInData&) = delete;

    void virtual send(ofxOscSender &oscSender);
  };

  using ChannelData = std::unique_ptr<SenderInData>;
  using SenderInChannel = ofThreadChannel<ChannelData>;



  //
  // Send contours
  //
  class SenderCountoursCmd : public SenderInData{
    vector<ofxCvBlob> _contours;

    void send_sendingContours(ofxOscSender oscSender, int numContours);

    void send_countour(ofxOscSender oscSender, ofxCvBlob &contour);

  public:
    SenderCountoursCmd(vector<ofxCvBlob> contours);
    ~SenderCountoursCmd();

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
    ~SenderMouseCmd();

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
    SenderInChannel _senderOut;
    // ofThreadChannel<>

    Sender _sender;

  public:
    Communicator();
    ~Communicator();

    void setup();

    void exit();

    void send(vector<ofxCvBlob> &contours);

    void send(int x, int y);
  };
}
