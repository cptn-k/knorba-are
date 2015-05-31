//
//  TextUiAgent.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 10/10/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#include <cstdlib>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <kfoundation/Logger.h>
#include <knorba/Runtime.h>
#include <knorba/MessageSet.h>

#include "TextUiAgent.h"

#define READ_END 0
#define WRITE_END 1

using namespace kfoundation;


const StaticPtr<KString> TextUiAgent::OP_SAY(new KString("knorba.demo.text.print"));
const StaticPtr<KString> TextUiAgent::OP_INIT(new KString("knorba.demo.textuiagent.init"));
const StaticPtr<KString> TextUiAgent::OP_QUIT(new KString("knorba.demo.textuiagent.quit"));
const StaticPtr<KString> TextUiAgent::OP_DISPLAYINFO_QUERY(new KString("knorba.displayinfo.query_local"));


TextUiAgent::TextUiAgent(k_guid_t guid, k_integer_t cloneId)
: Agent(guid, cloneId),
  _startingUi(false)
{
  _childId = -10;
}


TextUiAgent::~TextUiAgent() {
  quit();
}


char* cstr(const string& str) {
  char* buffer = new char[str.length() + 1];
  memcpy(buffer, str.c_str(), str.length() + 1);
  return buffer;
}


void TextUiAgent::startUi(int x, int y, int w, int h) {
  Ptr<Path> resourcePath = getResourcePath();
  AutoPtr<Path> jarPath = resourcePath->addSegement("TextUI.jar");
  
  if(pipe(_pipe) == -1) {
    perror("Error creating pipe");
  }
  
  int childId = fork();
  if (childId == 0) {  // Child goes here:
    
    // Closing unneeded write end
    if(close(_pipe[WRITE_END]) == -1) {
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
    
  } else if (childId > 0) { // Parent goes here:
    
    _childId = childId;
    
    if(close(_pipe[READ_END]) == -1) { // Close unused read end
      perror("Error closing pipe");
    }
    
    _startingUi = false;
    
    int status;
    wait(&status);
    
    LOG << "UI exited with status: " << status << EL;
    
    close(_pipe[WRITE_END]);
    quit();
    
  } else { // On error:
    
    close(_pipe[WRITE_END]);
    close(_pipe[READ_END]);
    perror("Error forking");
    quit();
    
  }
}


void TextUiAgent::handleMessage(Ptr<Message> msg) {
  if(msg->is(OP_SAY)) {
    
    while(_startingUi) {
      usleep(100000);
    }
    
    string text = HARD_CAST(msg->getPayload(), KString)->toUtf8String();
    
    LOG << "Text to display: \"" << text << "\"" << EL;
    
    text = text.append("\n");
    write(_pipe[WRITE_END], text.c_str(), text.size());
    
  } else if(msg->is(OP_INIT)) {
    
    _startingUi = true;
    
    AutoPtr<MessageSet> responses = sendToLocalsAndWait(OP_DISPLAYINFO_QUERY, KValue::NOTHING, 10000);
    
    if(!responses->isEmpty()) {
      
      Ptr<KRecord> config = HARD_CAST(responses->get(0)->getPayload(), KRecord);
      Ptr<KGrid> displays = HARD_CAST(config->member("displays"), KGrid);
      
      if(displays->getDimensions().get(0) > 0) {
        AutoPtr<KRecord> display1 = HARD_CAST(displays->at(Tuple1D(0)), KRecord);
        Ptr<KRecord> size = HARD_CAST(display1->member("size"), KRecord);
        Ptr<KRecord> offset = HARD_CAST(display1->member("offset"), KRecord);
        
        int x = HARD_CAST(offset->member(0), KInteger)->get();
        int y = HARD_CAST(offset->member(1), KInteger)->get();
        int w = HARD_CAST(size->member(0), KInteger)->get();
        int h = HARD_CAST(size->member(1), KInteger)->get();
        
        startUi(x, y, w, h);
      } else {
        LOG << "NO displays. Quitting." << EL;
        quit();
      }
      
    } else {
      
      LOG << "NO displays. Quitting." << EL;
      quit();
      
    }
    
    _startingUi = false;
    
  } else if(msg->is(OP_QUIT)) {
    
    quit();
    
  }
}


void TextUiAgent::quit() {
  if(_childId > 0) {
    close(_pipe[WRITE_END]);
    kill(_childId, SIGTERM);
  }
  Agent::quit();
}


KF_EXPORT
void init() {
  Runtime::get().registerMessageFormat(TextUiAgent::OP_SAY, KType::STRING);
  Runtime::get().registerMessageFormat(TextUiAgent::OP_INIT, KType::NOTHING);
}


KF_EXPORT
Agent* instantiate(k_guid_t guid, k_integer_t cloneId) {
  return new TextUiAgent(guid, cloneId);
}
