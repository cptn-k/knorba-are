//
//  CircleUiAgent.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 10/16/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#include <sstream>
#include <cmath>
#include <cstdlib>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <kfoundation/Logger.h>
#include <knorba/Runtime.h>
#include <knorba/MessageSet.h>

#include "Rect.h"
#include "Circle.h"

#include "CircleUiAgent.h"

#define READ_END 0
#define WRITE_END 1

using namespace kfoundation;

const SPtr<KString> CircleUiAgent::OP_INIT   = new KStringVal("circleui.init");
const SPtr<KString> CircleUiAgent::OP_QUIT   = new KStringVal("circleui.quit");
const SPtr<KString> CircleUiAgent::OP_AREA_Q = new KStringVal("circleui.area_q");
const SPtr<KString> CircleUiAgent::OP_AREA_A = new KStringVal("circleui.area_a");
const SPtr<KString> CircleUiAgent::OP_DRAW   = new KStringVal("circleui.draw");

const SPtr<KString> CircleUiAgent::OP_DISPLAY_INFO_QUERY
    = new KStringVal("knorba.displayinfo.query_local");

const SPtr<KString> CircleUiAgent::OP_DISPLAY_INFO_INFORM
    = new KStringVal("knorba.displayinfo.inform");


CircleUiAgent::CircleUiAgent(const k_guid_t& guid, const k_integer_t cloneId)
: Agent(guid, cloneId)
{
  _startingUi = false;
  _childId = -10;
  _translateX = 0;
  _translateY = 0;
}


CircleUiAgent::~CircleUiAgent() {
  // Nothing;
}


char* cstr(const string& str) {
  char* buffer = new char[str.length() + 1];
  memcpy(buffer, str.c_str(), str.length() + 1);
  return buffer;
}


void CircleUiAgent::startUi(int x, int y, int w, int h) {
  PPtr<Path> resourcePath = getResourcePath();
  Ptr<Path> jarPath = resourcePath->addSegement("CircleUI.jar");
  
  cout << "Jar Path: " << *jarPath << endl;
  
  if(pipe(_pipe) == -1) {
    perror("Error creating pipe");
  }

  int childId = fork();
  if (childId == 0) {
    if(close(_pipe[WRITE_END]) == -1) { // Close unused write end
      perror("Error closing pipe");
    }
    
    dup2(_pipe[READ_END], STDIN_FILENO);
    
    const char* command = "java";
    
    char* args[10];
    args[0] = cstr(command);
    args[1] = cstr("-jar");
    args[2] = cstr(jarPath->toString());
    args[3] = cstr(Int(x).toString());
    args[4] = cstr(Int(y).toString());
    args[5] = cstr(Int(w).toString());
    args[6] = cstr(Int(h).toString());
    args[7] = NULL;
    
    execvp(command, args);
    perror("exec of the child process");
    exit(0);
  } else if (childId > 0) {
    _childId = childId;
    
    if(close(_pipe[READ_END]) == -1) { // Close unused read end
      perror("Error closing pipe");
    }
    
    _startingUi = false;
    
    int status;
    //::wait(&status);
    usleep(100000000);
    
    LOG << "UI exited with status: " << status << EL;
    
    close(_pipe[WRITE_END]);
    quit();
  } else {
    close(_pipe[WRITE_END]);
    close(_pipe[READ_END]);
    perror("Error forking");
  }
  
}

void CircleUiAgent::handleMessage(PPtr<Message> msg) {
  if(msg->is(OP_INIT)) {
    
    _startingUi = true;
    
    Ptr<MessageSet> responses = tsendToLocals(OP_DISPLAY_INFO_QUERY,
        KValue::NOTHING, 10000);
    
    if(responses->isEmpty()) {
      LOG << "Display info not available. Quitting." << EL;
      quit();
      return;
    }
    
    PPtr<KRecord> config = responses->get(0)->getPayload().AS(KRecord);
    PPtr<KGridBasic> displays = config->field("displays").AS(KGridBasic);
    
    if(displays->getDimensions().get(0) > 0) {
      PPtr<KRecord> display1 = displays->at(Tuple1D(0)).AS(KRecord);
      
      PPtr<KRecord> size = display1->field("size").AS(KRecord);
      PPtr<KRecord> offset = display1->field("offset").AS(KRecord);
      PPtr<KRecord> position = display1->field("globalPosition").AS(KRecord);
      
      _translateX = position->field(0).AS(KInteger)->get();
      _translateY = position->field(1).AS(KInteger)->get();
      int windowX = offset->field(0).AS(KInteger)->get();
      int windowY = offset->field(1).AS(KInteger)->get();
      int w = size->field(0).AS(KInteger)->get();
      int h = size->field(1).AS(KInteger)->get();
      
      _displayRect.replace(new Rect());
      _displayRect->set(_translateX, _translateY, w, h);
      
      startUi(windowX, windowY, w, h);
    } else {
      LOG << "No displays. Using standard console instead." << EL;
    }
    _startingUi = false;
    
  } else if(msg->is(OP_DRAW)) {
    
    while(_startingUi) {
      usleep(100000);
    }
    
    PPtr<KGridBasic> circles = msg->getPayload().AS(KGridBasic);
    PPtr<Circle> c(new Circle());
    
    k_longint_t n = circles->getDimensions().get(0);
    
    stringstream sstr;
    
    sstr.put('c');
    
    for(int i = 0; i < n; i++) {
      c->set(circles->at(Tuple1D(i)).AS(KRecord));
      sstr << ((int)c->getX() - _translateX);
      sstr.put('x');
      sstr << ((int)c->getY() - _translateY);
      sstr.put('y');
      sstr << (int)c->getR();
      sstr.put('r');
      sstr.put('a');
    }
    
    sstr.put('\n');
    
    string text = sstr.str();
    
    write(_pipe[WRITE_END], text.c_str(), text.size());
    
  } else if(msg->is(OP_AREA_Q)) {
    
    respond(msg, OP_AREA_A, _displayRect.AS(KValue));
    
  } else if(msg->is(OP_QUIT)) {
    
    quit();
    
  }
}

void CircleUiAgent::quit() {
  if(_childId > 0) {
    close(_pipe[WRITE_END]);
    kill(_childId, SIGTERM);
  }
  Agent::quit();
}

KF_EXPORT
void init() {
  SPtr<KType> gridOfCircles(new KGridType(Circle::type().AS(KType), 1));
  
//  Runtime::get().registerType(gridOfCircles);
//  Runtime::get().registerType(Rect::type().AS(KType));
  
  Runtime::get().registerMessageFormat(CircleUiAgent::OP_INIT, KType::NOTHING);
  Runtime::get().registerMessageFormat(CircleUiAgent::OP_DRAW, gridOfCircles.AS(KType));
  Runtime::get().registerMessageFormat(CircleUiAgent::OP_AREA_Q, KType::NOTHING);
  Runtime::get().registerMessageFormat(CircleUiAgent::OP_AREA_A, Rect::type().AS(KType));
}

KF_EXPORT
Agent* instantiate(const k_guid_t& guid, const k_integer_t cloneId) {
  return new CircleUiAgent(guid, cloneId);
}
