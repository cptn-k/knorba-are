//
//  UiTextOutputAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/16/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// Std
#include <cstdlib>

// UNIX
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// KnoRBA
#include "StartProtocol.h"

// Internal
#include "TextOutputProtocol.h"

// Self
#include "UiTextOutputAgent.h"

//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

KF_EXPORT
void init(Runtime& rt) {
  TextOutputProtocol::init(rt);
  StartProtocol::init(rt);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new UiTextOutputAgent(rt, guid);
}


namespace {
  char* cstr(const string& str) {
    char* buffer = new char[str.length() + 1];
    memcpy(buffer, str.c_str(), str.length() + 1);
    return buffer;
  }
}


//\/ UiTextOutputAgent /\//////////////////////////////////////////////////////

// --- STATIC METHODS --- //

void* UiTextOutputAgent::runJarRunner(void* arg) {
  ((UiTextOutputAgent*)arg)->runJar();
  return NULL;
}


UiTextOutputAgent::UiTextOutputAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid),
  _pDisplayInfo(this, true)
{
  _uiReady = false;
}


void UiTextOutputAgent::runJar() {
  _pDisplayInfo.queryLocalDisplaySetup();
  
  if(!_pDisplayInfo.hasDisplay()) {
    log(Logger::ERR) << "No displays on this node." << EL;
    return;
  } else {
    _isFullScreen = _pDisplayInfo.isFullScreen();
    _windowBounds = _pDisplayInfo.getBounds();
    _windowOrigin = _pDisplayInfo.getWindowOrigin();
  }
  
  PPtr<Path> resourcePath = getResourcePath();
  Ptr<Path> jarPath = resourcePath->addSegement("TextUI.jar");
  
  if(pipe(_pipe) == -1) {
    perror("Error creating pipe");
    return;
  }
  
  log() << "Startng UI: " << jarPath->getString() << EL;
  
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
    args[2] = cstr(jarPath->getString());
    args[3] = cstr(Int(_windowOrigin.at(0)).toString());
    args[4] = cstr(Int(_windowOrigin.at(1)).toString());
    args[5] = cstr(Int(_windowBounds.getSize().at(0)).toString());
    args[6] = cstr(Int(_windowBounds.getSize().at(1)).toString());
    args[7] = NULL;
    
    execvp(command, args);
    perror("execvp()");
    exit(0);
  } else if (childId > 0) {
    _childId = childId;
    
    if(close(_pipe[READ_END]) == -1) { // Close unused read end
      perror("Error closing pipe");
    }
    
    System::usleep(900000);
    _uiReady = true;
    
    int status;
    ::wait(&status);
    
    _uiReady = false;

    log() << "UI exited with status: " << status << EL;

    close(_pipe[WRITE_END]);
    quit();
  } else {
    close(_pipe[WRITE_END]);
    close(_pipe[READ_END]);
    perror("fork()");
  }
}


void UiTextOutputAgent::handleMessage(PPtr<Message> msg) {
  if(msg->is(TextOutputProtocol::OP_PUT)) {
    
    if(_uiReady) {
      string text = msg->getPayload().AS(KString)->toUtf8String() + "\n";
      write(_pipe[WRITE_END], text.c_str(), text.size());
    } else {
      cout << *msg->getPayload() << endl;
    }
    
  } else if(msg->is(TextOutputProtocol::OP_PING)) {
    
    respond(msg, TextOutputProtocol::OP_PING, KValue::NOTHING);
    
  } else if(msg->is(StartProtocol::OP_START)) {
    
    System::makeDetachedThread(&UiTextOutputAgent::runJarRunner, this);
    
  }
}


void UiTextOutputAgent::finalize() {
  _uiReady = false;
  if(_childId > 0) {
    close(_pipe[WRITE_END]);
    kill(_childId, SIGTERM);
  }
  Agent::finalize();
}