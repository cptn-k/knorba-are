//
//  TextOutputProtocol.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/16/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/MessageSet.h>

// Self
#include "TextOutputProtocol.h"

// --- STATIC FIELDS --- //

SPtr<KString> TextOutputProtocol::OP_PING
    = new KString("knorba.demo.text-output.ping");

SPtr<KString> TextOutputProtocol::OP_PUT
    = new KString("knorba.demo.text-output.put");


// --- STATIC METHODS --- //

void TextOutputProtocol::init(Runtime &rt) {
  rt.registerMessageFormat(OP_PING, KType::NOTHING);
  rt.registerMessageFormat(OP_PUT, KType::STRING);
}


// --- CONSTURCTORS --- //

TextOutputProtocol::TextOutputProtocol(Agent* agent)
: Protocol(agent)
{
  _servers = new Group();
}


// --- METHODS --- //

bool TextOutputProtocol::connectToServers() {
  Ptr<MessageSet> msg = getOwner()->tsendToLocals(OP_PING, KValue::NOTHING, 10000);
  _servers->add(msg->getSenders());
  return _servers->getCount() > 0;
}


PPtr<Group> TextOutputProtocol::getServers() {
  return _servers;
}


void TextOutputProtocol::put(const string& text) {
  if(_servers->getCount() == 0) {
    cout << text << endl;
  }
  
  Ptr<KString> payload = new KString(text);
  getOwner()->send(_servers, OP_PUT, payload.AS(KValue));
}

