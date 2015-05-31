//
//  PingPongAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/22/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// Std
#include <cstdlib>

// KFoundation
#include <kfoundation/Ptr.h>

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/Group.h>
#include <knorba/type/all.h>

// Protocol
#include <knorba/protocol/GroupingProtocol.h>
#include <knorba/protocol/ConsoleProtocolClient.h>

// Self
#include "PingPongAgent.h"

//\/ PingPongAgent::PGrouping /\///////////////////////////////////////////////

PingPongAgent::PGrouping::PGrouping(PingPongAgent* owner)
: GroupingProtocol(owner, PingPongAgent::R_MEMBER, 1),
  _iAgent(owner)
{
  // Nothing;
}


void PingPongAgent::PGrouping::onMemberDisconnected(const k_guid_t& guid) {
  if(getNMembers() == 1) {
    _iAgent->quit();
  }
}


void PingPongAgent::PGrouping::onAllMembersConnected() {
  if(getNMembers() == 1) {
    _iAgent->_pConsole.print("No other peers. Quitting.");
    _iAgent->quit();
  } else {
    if(_iAgent->getRuntime().isHead()) {
      _iAgent->pingRandom();
    }
  }
}


// --- STATIC FIELDS --- //

const SPtr<KString> PingPongAgent::OP_START = KS("knorba.demo.pingpong.start");
const SPtr<KString> PingPongAgent::OP_PING = KS("knorba.demo.pingpong.ping");
const SPtr<KString> PingPongAgent::OP_FINISH = KS("knorba.demo.pingpong.finish");
const SPtr<KString> PingPongAgent::R_MEMBER = KS("member");


// --- (DE)CONSTRUCTORS --- //

PingPongAgent::PingPongAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid),
  _pConsole(this),
  _pGrouping(this)
{
  _pConsole.setServer(getRuntime().getConsoleGuid());
  
  pingCounter = new KInteger(0);
  Agent::registerHandler((handler_t)&PingPongAgent::handleOpStart, OP_START);
  Agent::registerHandler((handler_t)&PingPongAgent::handleOpPing, OP_PING);
  Agent::registerHandler((handler_t)&PingPongAgent::handleOpFinish, OP_FINISH);
}


PingPongAgent::~PingPongAgent() {
  // Nothing;
}


// --- METHODS --- //

void PingPongAgent::pingRandom() {
  PPtr<Group> g = getPeers(R_MEMBER);
  int index = rand() % g->getCount();
  const k_guid_t& target = g->get(index);
  _pConsole.print("ping(" + Int::toString(pingCounter->get()) + ")");
  send(target, OP_PING, pingCounter.AS(KValue));
}


// Handlers //

void PingPongAgent::handleOpStart(PPtr<Message> msg) {
  _pGrouping.start();
}


void PingPongAgent::handleOpPing(PPtr<Message> msg) {
  pingCounter->set(msg->getPayload());
  pingCounter->set(pingCounter->get() + 1);
  
  System::sleep(DELAY);
  
  if(pingCounter->get() > 20) {
    send(R_MEMBER, OP_FINISH, KValue::NOTHING);
    quit();
  } else {
    pingRandom();
  }
}


void PingPongAgent::handleOpFinish(PPtr<Message> msg) {
  quit();
}


void mpiExample() {
  
}


KF_EXPORT
void init(Runtime& rt) {
  ConsoleProtocolClient::init(rt);
  GroupingProtocol::init(rt);
  rt.registerMessageFormat(PingPongAgent::OP_START, KType::NOTHING);
  rt.registerMessageFormat(PingPongAgent::OP_PING, KType::INTEGER);
  rt.registerMessageFormat(PingPongAgent::OP_FINISH, KType::NOTHING);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new PingPongAgent(rt, guid);
}
