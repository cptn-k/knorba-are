//
//  PicServerAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/27/15.
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
#include <kfoundation/Path.h>
#include <kfoundation/Tuple.h>
#include <kfoundation/Range.h>

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/type/all.h>
#include <knorba/protocol/GroupingProtocol.h>
#include <knorba/protocol/DisplayInfoProtocol.h>


// Internal
#include "../shared/Point.h"
#include "../shared/Rectangle.h"
#include "PicProtocol.h"

// Self
#include "PicServerAgent.h"

//\/ PicServerAgent::PGrouping /\//////////////////////////////////////////////

PicServerAgent::PGrouping::PGrouping(PicServerAgent* owner)
: GroupingProtocol(owner, PicServerAgent::R_MATE, 1)
{
  // Nothing
}


void PicServerAgent::PGrouping::onAllMembersConnected() {
  // Nothing yet
}


//\/ PicServerAgent::JarRunnerThread /\////////////////////////////////////////

PicServerAgent::JarRunnerThread::JarRunnerThread(PicServerAgent* agent)
: Thread(agent->getAlias() + " jar runner")
{
  _iAgent = agent;
}


void PicServerAgent::JarRunnerThread::run() {
  _iAgent->runJar();
}


//\/ PicServerAgent /\/////////////////////////////////////////////////////////

// --- STATIC FIELDS --- //

const SPtr<KString> PicServerAgent::OP_START = KS("knorba.demo.pic.server.start");
const SPtr<KString> PicServerAgent::R_MATE = KS("mate");


// ---- (DE)CONSTRUCTORS --- //

PicServerAgent::PicServerAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid),
  _pDisplayInfo(this)
{
  _jarRunnerThread = new JarRunnerThread(this);
  
  registerHandler((handler_t)&PicServerAgent::handleOpStart, OP_START);
  registerHandler((handler_t)&PicServerAgent::handleOpLoad, PicProtocol::OP_LOAD);
  registerHandler((handler_t)&PicServerAgent::handleOpPut, PicProtocol::OP_PUT);
  registerHandler((handler_t)&PicServerAgent::handleOpUnput, PicProtocol::OP_UNPUT);
}


// --- METHODS --- //

void PicServerAgent::runJar() {
  _pDisplayInfo.queryLocalDisplays();
  
  if(!_pDisplayInfo.hasDisplay()) {
    ALOG_ERR << "No displays on this node." << EL;
    return;
  }
  
  PPtr<Path> resourcePath = getPathToResources();
  Ptr<Path> jarPath = resourcePath->addSegement("PicUI.jar");
  
  if(pipe(_pipe) == -1) {
    ALOG_ERR << "Error creating pipe" << EL;
    return;
  }
  
  ALOG << "Startng UI: " << jarPath->getString() << EL;

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
    
    const Range& rect = _pDisplayInfo.getLocalInfo()->getRect();
    const Tuple& origin = _pDisplayInfo.getWindowOrigin();
    
    stringstream sstr;
    sstr << 'w' << origin.at(0) << ' ' << origin.at(1)
    << ' ' << rect.getBegin().at(0) << ' ' << rect.getBegin().at(1)
    << ' ' << rect.getSize().at(0) << ' ' << rect.getSize().at(1) << '#';
    
    writeToPipe(sstr.str());
    
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
  
  _uiReady = false;
}


void PicServerAgent::writeToPipe(const string& str) {
  ALOG << str << EL;
  write(_pipe[WRITE_END], str.c_str(), str.length());
}


// Handlers //

void PicServerAgent::handleOpStart(PPtr<Message> msg) {
  _jarRunnerThread->start();
}


void PicServerAgent::handleOpLoad(PPtr<Message> msg) {
  PPtr<KRecord> r = msg->getPayload().AS(KRecord);
  
  Ptr<Path> path = getPathToData()->addSegement(
      r->getString(PicProtocol::LOAD_T_FILENAME)->toUtf8String());
  
  r->getRaw(PicProtocol::LOAD_T_DATA)->writeDataToFile(path);
  
  stringstream sstr;
  sstr << 'l' << r->getInteger(PicProtocol::LOAD_T_REFERENCE) << ' '
      << path->getString() << '#';
  
  writeToPipe(sstr.str());
}


void PicServerAgent::handleOpPut(PPtr<Message> msg) {
  PPtr<KRecord> r = msg->getPayload().AS(KRecord);
  Rectangle rect(r->getRecord(PicProtocol::PUT_T_AREA));
  
  stringstream sstr;
  sstr << 'p' << r->getInteger(PicProtocol::PUT_T_REFERENCE)
    << ' ' << rect.getBegin().at(0) << ' ' << rect.getBegin().at(1)
    << ' ' << rect.getEnd().at(0) << ' ' << rect.getEnd().at(1) << '#';
  
  writeToPipe(sstr.str());
}


void PicServerAgent::handleOpUnput(PPtr<Message> msg) {
  writeToPipe('d' + msg->getPayload().AS(KInteger)->toString() + '#');
}


// Inherited from Agent //

void PicServerAgent::finalize() {
  if(_childId > 0) {
    close(_pipe[WRITE_END]);
    kill(_childId, SIGTERM);
  }
  Agent::finalize();
}


bool PicServerAgent::isAlive() {
  return Agent::isAlive() || _jarRunnerThread->isRunning();
}


//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

KF_EXPORT
void init(Runtime& rt) {
  rt.registerMessageFormat(PicServerAgent::OP_START, KType::NOTHING);
  GroupingProtocol::init(rt);
  PicProtocol::init(rt);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new PicServerAgent(rt, guid);
}