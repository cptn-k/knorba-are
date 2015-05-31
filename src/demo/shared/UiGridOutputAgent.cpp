//
//  UiGridOutputAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Kay Khandan on 3/8/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// Std
#include <cstdlib>

// UNIX
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/RangeIterator.h>
#include <kfoundation/Path.h>

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/Runtime.h>
#include <knorba/type/all.h>

// Internal
#include <knorba/protocol/DisplayInfoProtocol.h>
#include "PixmapOutputProtocol.h"

// Self
#include "UiGridOutputAgent.h"

//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

KF_EXPORT
void init(Runtime& rt) {
  rt.registerMessageFormat(UiGridOutputAgent::OP_START, KType::NOTHING);
  PixmapOutputProtocol::init(rt);
  DisplayInfoProtocol::init(rt);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new UiGridOutputAgent(rt, guid);
}


namespace {
  char* cstr(const string& str) {
    char* buffer = new char[str.length() + 1];
    memcpy(buffer, str.c_str(), str.length() + 1);
    return buffer;
  }
}

//\/ UiGridOutputAgent /\//////////////////////////////////////////////////////

// --- STATIC FIELDS --- //

const SPtr<KString> UiGridOutputAgent::OP_START
    = KS("knorba.demo.ui-grid-output.start");


// --- STATIC METHODS --- //

double UiGridOutputAgent::interpolate(double val, double y0, double x0,
    double y1, double x1)
{
  return (val-x0)*(y1-y0)/(x1-x0) + y0;
}


double UiGridOutputAgent::blue(double grayscale) {
  if(grayscale < -0.33) {
    return 1.0;
  }
  
  if(grayscale < 0.33) {
    return interpolate(grayscale, 1.0, -0.33, 0.0, 0.33);
  }
  
  return 0.0;
}


double UiGridOutputAgent::green(double grayscale) {
  if(grayscale < -1.0) {
    return 0.0; // unexpected grayscale value
  }
  
  if(grayscale < -0.33) {
    return interpolate(grayscale, 0.0, -1.0, 1.0, -0.33);
  }
  
  if(grayscale < 0.33) {
    return 1.0;
  }
  
  if(grayscale <= 1.0) {
    return interpolate(grayscale, 1.0, 0.33, 0.0, 1.0);
  }
  
  return 1.0; // unexpected grayscale value
}


double UiGridOutputAgent::red(double grayscale) {
  if(grayscale < -0.33) {
    return 0.0;
  }
  
  if(grayscale < 0.33) {
    return interpolate(grayscale, 0.0, -0.33, 1.0, 0.33);
  }
  
  return 1.0;
}


// --- (DE)CONSTRUCTORS --- //

UiGridOutputAgent::UiGridOutputAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid),
  _pDisplayInfo(this)
{
  _uiReady = false;
  _isWindowSet = false;
  _isUiRunning = false;
  generateColorMaps();
  
  registerHandler((handler_t)&UiGridOutputAgent::handleOpPut,
      PixmapOutputProtocol::OP_PUT);
  
  registerHandler((handler_t)&UiGridOutputAgent::handleOpStart, OP_START);
}


void UiGridOutputAgent::generateColorMaps() {
  // BLACK_AND_WHITE
  _colorMapBW[0].set(245, 245, 255);
  _colorMapBW[1].set(40, 40, 255);
  
  // Color 16
	_colorMap16[0].set(0, 0, 0);
	_colorMap16[1].set(157, 157, 157);
	_colorMap16[2].set(255, 255, 255);
	_colorMap16[3].set(190, 38, 51);
	_colorMap16[4].set(224, 111, 139);
	_colorMap16[5].set(73, 60, 43);
	_colorMap16[6].set(164, 100, 34);
	_colorMap16[7].set(235, 137, 49);
	_colorMap16[8].set(247, 226, 107);
	_colorMap16[9].set(47, 72, 78);
	_colorMap16[10].set(68, 137, 26);
	_colorMap16[11].set(163, 206, 39);
	_colorMap16[12].set(27, 38, 50);
	_colorMap16[13].set(0, 87, 132);
	_colorMap16[14].set(49, 162, 242);
	_colorMap16[15].set(178, 220, 239);
  
  // Color 256
  for(int i = 0; i < 256; i++) {
    double gray = i/255.0;
    _colorMap256[i].set((int)(red(gray) * 255), (int)(green(gray)*255),
                        (int)(green(gray)*255));
  }
  
}


void UiGridOutputAgent::runJar() {
  _pDisplayInfo.queryLocalDisplays();
  
  if(!_pDisplayInfo.hasDisplay()) {
    ALOG_ERR << "No displays on this node." << EL;
    return;
  }
  
  _isFullScreen = _pDisplayInfo.isFullScreen();
  _windowSize = _pDisplayInfo.getLocalInfo()->getRect().getSize();
  _windowOrigin = _pDisplayInfo.getWindowOrigin();
  
  PPtr<Path> resourcePath = getPathToResources();
  Ptr<Path> jarPath = resourcePath->addSegement("GridUI.jar");
  
  if(pipe(_pipe) == -1) {
    ALOG_ERR << "Error creating pipe" << EL;
    return;
  }

  ALOG << "Startng UI: " << jarPath->getString() << EL;
  
  _isUiRunning = true;
  
  int childId = fork();
  if (childId == 0) { // Child
    if(close(_pipe[WRITE_END]) == -1) { // Close unused write end
      ALOG << "Error closing piple" << EL;
    }
    
    dup2(_pipe[READ_END], STDIN_FILENO);
    
    const char* command = "java";
    
    char* args[5];
    args[0] = cstr(command);
    args[1] = cstr("-jar");
    args[2] = cstr(jarPath->getString());
    args[3] = NULL;
    
    execvp(command, args);
    perror("execvp()");
    exit(0);
  } else if (childId > 0) { // Parent
    _childId = childId;
    
    if(close(_pipe[READ_END]) == -1) { // Close unused read end
      ALOG_ERR << "Error closing piple" << EL;
    }
    
    _uiReady = true;
    
    int status;
    ::wait(&status);
    
    ALOG << "UI exited with status: " << status << EL;
    
    close(_pipe[WRITE_END]);
    quit();
  } else {
    close(_pipe[WRITE_END]);
    close(_pipe[READ_END]);
    perror("fork()");
  }
  
  _isUiRunning = false;
}


void UiGridOutputAgent::put(k_octet_t format, PPtr<KGrid> data) {
  if(!_uiReady) {
    return;
  }
  
  _uiReady = false;
  
  // windowX windowY windowWidth windowHeight gridWidth gridHeight
  // r000g000b000 # q
  
  stringstream sstr;
  
  if(!_isWindowSet) {
    if(_isFullScreen) {
      sstr << "f ";
    } else {
      sstr << 'w' << _windowOrigin.at(0) << ' ' << _windowOrigin.at(1) << ' '
          << _windowSize.at(0) << ' ' << _windowSize.at(1) << ' ';
    }
    _isWindowSet = true;
  }
  
  const Tuple& size = data->getRange().getSize();
  
  sstr << size.at(0) << ' ' << size.at(1) << ' ';

  Ptr<KRecord> r = new KRecord(data);
  for (RangeIterator it(data->getRange()); it.hasMore(); it.next()) {
    k_octet_t v = data->at(it, r)->getOctet();
    Color c;
    switch ((PixmapOutputProtocol::output_format_t)format) {
      case PixmapOutputProtocol::BLACK_AND_WHITE:
        if(v == 0) {
          c = _colorMapBW[0];
        } else {
          c = _colorMapBW[1];
        }
        break;
        
      case PixmapOutputProtocol::COLOR_16:
        c = _colorMap16[v];
        break;
        
      case PixmapOutputProtocol::GRAY_16:
        c.set(v*16, v*16, v*16);
        break;
        
      case PixmapOutputProtocol::GRAY_256:
        c = _colorMap256[v];
        break;
    }
    
    sstr << 'r' << (int)c.red << 'g' << (int)c.green << 'b' << (int)c.blue;
  }
  
  sstr.put('#');

  string text = sstr.str();
  
  write(_pipe[WRITE_END], text.c_str(), text.size());
  
  _uiReady = true;
}


void UiGridOutputAgent::handleOpPut(PPtr<Message> msg) {
  PPtr<KRecord> r = msg->getPayload().AS(KRecord);
  k_octet_t format = r->getEnumerationOrdinal(PixmapOutputProtocol::PUT_T_FORMAT);
  PPtr<KGrid> data = r->field<KGrid>(PixmapOutputProtocol::PUT_T_DATA);
  put(format, data);
  
  if(msg->needsResponse()) {
    respond(msg, OP_ACK, KValue::NOTHING);
  }
}


void UiGridOutputAgent::handleOpStart(PPtr<Message> msg) {
  runJar();
}


void UiGridOutputAgent::finalize() {
  if(_childId > 0) {
    close(_pipe[WRITE_END]);
    kill(_childId, SIGTERM);
  }
  Agent::finalize();
}

bool UiGridOutputAgent::isAlive() {
  return Agent::isAlive() || _isUiRunning;
}

