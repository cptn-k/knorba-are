//
//  SingInSyncAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/23/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// Std
#include <cstdlib>

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/Thread.h>
#include <kfoundation/Tuple.h>
#include <kfoundation/DirectionIterator.h>
#include <kfoundation/RangeIterator.h>
#include <kfoundation/ProximityIterator.h>


// KnoRBA
#include <knorba/Agent.h>
#include <knorba/Group.h>
#include <knorba/type/all.h>
#include <knorba/MessageSet.h>

// Protocol
#include <knorba/protocol/GroupingProtocol.h>
#include <knorba/protocol/PhaserProtocol.h>
#include <knorba/protocol/ConsoleProtocolClient.h>

#include "SingInSyncAgent.h"

//\/ SingInSyncAgent::PGrouping /\/////////////////////////////////////////////

SingInSyncAgent::PGrouping::PGrouping(SingInSyncAgent* agent)
: GroupingProtocol(agent, SingInSyncAgent::R_MATE, 1),
  _iAegnt(agent)
{
  // Nothing;
}


void SingInSyncAgent::PGrouping::onAllMembersConnected() {
  _iAegnt->_thread->start();
}


//\/ SingInSyncAgent::SingThread /\////////////////////////////////////////////

SingInSyncAgent::SingThread::SingThread(SingInSyncAgent* agent) {
  _iAgent = agent;
}


void SingInSyncAgent::SingThread::run() {
  _iAgent->singLoop();
}


//\/ SingInSyncAgent /\////////////////////////////////////////////////////////

// --- STATIC FIELDS --- //

SPtr<KGridType> SingInSyncAgent::LYRICS_T;
const SPtr<KString> SingInSyncAgent::OP_SING
    = KS("konrba.demo.sing-in-sync.sing");

const SPtr<KString> SingInSyncAgent::R_MATE = KS("mate");


// --- STATIC METHODS --- //

SPtr<KGridType> SingInSyncAgent::lyrics_t() {
  if(LYRICS_T.isNull()) {
    LYRICS_T = new KGridType(KType::STRING, 1);
  }
  return LYRICS_T;
}


// --- (DE)CONSTRUCTORS --- //

SingInSyncAgent::SingInSyncAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid),
  _pConsole(this),
  _pGrouping(this),
  _pPhaser(this, R_MATE)
{
  _thread = new SingThread(this);
  _stopFlag = false;
  
  _pConsole.setServer(getRuntime().getConsoleGuid());
  
  registerHandler((handler_t)&SingInSyncAgent::handleOpSing, OP_SING);
}


// --- METHODS --- //

void SingInSyncAgent::singLoop() {
  ALOG << "Sing loop started." << EL;
  
  Ptr<KRecord> rec = new KRecord(_lyrics);
  
  for(RangeIterator i(_lyrics->getRange()); i.hasMore() && !_stopFlag; i.next()) {
    System::sleep(WAIT/2 + rand()%WAIT/2);
    _pPhaser.synchronize();
    _pConsole.print(_lyrics->at(i, rec)->getString());
  }
  
  if(!_stopFlag) {
    System::sleep(WAIT);
    _pConsole.print("End.");
    quit();
  }
  
  ALOG << "Sing loop terminated." << EL;
}


void SingInSyncAgent::handleOpSing(PPtr<Message> msg) {
  _lyrics = msg->getPayload().AS(KGrid);
  _pGrouping.start();
}


bool SingInSyncAgent::isAlive() {
  return Agent::isAlive() || _thread->isRunning();
}


void SingInSyncAgent::finalize() {
  _stopFlag = true;
  Agent::finalize();
}


//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

KF_EXPORT
void init(Runtime& rt) {
  GroupingProtocol::init(rt);
  PhaserProtocol::init(rt);
  ConsoleProtocolClient::init(rt);
  
  rt.registerMessageFormat(SingInSyncAgent::OP_SING,
      SingInSyncAgent::lyrics_t().AS(KType));
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new SingInSyncAgent(rt, guid);
}
