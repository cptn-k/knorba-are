//
//  DisplayInfoAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/21/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>

// KnoRBA
#include <knorba/type/all.h>
#include <knorba/protocol/DisplayInfoProtocol.h>

// Self
#include "DisplayInfoAgent.h"

// --- (DE)CONSTRUCTORS --- //

DisplayInfoAgent::DisplayInfoAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid)
{
  registerHandler((handler_t)&DisplayInfoAgent::handleOpSetupQ,
      DisplayInfoProtocol::OP_SETUP_Q);
  
  registerHandler((handler_t)&DisplayInfoAgent::handleOpSetupA,
      DisplayInfoProtocol::OP_SETUP_A);
}


// --- METHODS --- //

// Handlers //

void DisplayInfoAgent::handleOpSetupQ(PPtr<Message> msg) {
  if(_localSetup.isNull()) {
    return;
  }
  respond(msg, DisplayInfoProtocol::OP_SETUP_A, _localSetup.AS(KValue));
}


void DisplayInfoAgent::handleOpSetupA(PPtr<Message> msg) {
  _localSetup = msg->getPayload().AS(KRecord);
}


KF_EXPORT
void init(Runtime& rt) {
  DisplayInfoProtocol::init(rt);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new DisplayInfoAgent(rt, guid);
}