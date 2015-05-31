//
//  UiGridOutputAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Kay Khandan on 3/8/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__UiGridOutputAgent__
#define __AgentRuntimeEnvironment__UiGridOutputAgent__


using namespace knorba;
using namespace knorba::type;
using namespace knorba::protocol;

//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

extern "C" void init(Runtime& rt);
extern "C" knorba::Agent* instantiate(Runtime& rt, const k_guid_t& guid);


//\/ UiGridOutputAgent /\//////////////////////////////////////////////////////

class UiGridOutputAgent : public Agent {

// --- NESTED TYPES --- //
  
  private: struct Color {
    public: k_octet_t red;
    public: k_octet_t green;
    public: k_octet_t blue;
    public: void set(k_octet_t r, k_octet_t g, k_octet_t b) {
      red = r;
      green = g;
      blue = b;
    }
  };
  
// --- STATIC FIELDS --- //
  
  private: static const int READ_END = 0;
  private: static const int WRITE_END = 1;
  public: static const SPtr<KString> OP_START;
  
  
// --- STATIC METHODS --- //
  
  private: static double interpolate(double val, double y0, double x0,
      double y1, double x1);
  
  private: static double blue(double grayscale);
  private: static double green(double grayscale);
  private: static double red(double grayscale);

  
// --- FIELDS --- //
  
  private: DisplayInfoProtocol _pDisplayInfo;
  private: Tuple _windowSize;
  private: Tuple _windowOrigin;
  private: bool _isFullScreen;
  private: int _pipe[2];
  private: int _childId;
  private: bool _uiReady;
  private: bool _isWindowSet;
  private: bool _isUiRunning;
  
  private: Color _colorMapBW[2];
  private: Color _colorMap16[16];
  private: Color _colorMap256[256];
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: UiGridOutputAgent(Runtime& rt, const k_guid_t& guid);
  
  
// --- METHODS --- //
  
  private: void generateColorMaps();
  public: void runJar();
  public: void put(k_octet_t format, PPtr<KGrid> data);
  public: void handleOpPut(PPtr<Message> msg);
  public: void handleOpStart(PPtr<Message> msg);
  public: void finalize();
  public: bool isAlive();
};

#endif /* defined(__AgentRuntimeEnvironment__UiGridOutputAgent__) */
