//
//  StartProtocol.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/4/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#include "StartProtocol.h"

// --- STATIC FIELDS --- //

SPtr<KString> StartProtocol::OP_START = new KString("knorba.start");


// --- STATIC METHODS --- //

void StartProtocol::init(Runtime &rt) {
  rt.registerMessageFormat(OP_START, KType::NOTHING);
}


// --- (DE)CONSTRUCTORS --- //

StartProtocol::StartProtocol(Agent* owner)
: Protocol(owner)
{
  // Nothing;
}


// --- METHODS --- //

bool StartProtocol::handleMessage(PPtr<Message> msg) {
  if(msg->is(OP_START)) {
    onStart();
    return true;
  }
  return false;
}
