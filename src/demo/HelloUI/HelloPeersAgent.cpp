//
//  HelloAgent.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 10/19/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#include <iostream>
#include <knorba/Runtime.h>

#include "HelloPeersAgent.h"

using namespace std;


const StaticPtr<KString> HelloPeersAgent::OP_LINK(new KString("knorba.demo.hellopeers.link"));
const StaticPtr<KString> HelloPeersAgent::OP_PRINT(new KString("knorba.demo.text.print"));


HelloPeersAgent::HelloPeersAgent(const k_guid_t guid, const k_integer_t cloneId)
: Agent(guid, cloneId),
  _peers(new Group())
{
  _peers.retain();
  registerHandler((handler)&HelloPeersAgent::opLinkHandler, OP_LINK);
  registerHandler((handler)&HelloPeersAgent::opSayHandler, OP_PRINT);
  setAutoExit();
}


HelloPeersAgent::~HelloPeersAgent() {
  _peers.release();
}


void HelloPeersAgent::opLinkHandler(Ptr<Message> msg) {
  _peers->add(msg->getPayload().AS(KGUID)->get());
}


void HelloPeersAgent::opSayHandler(Ptr<Message> msg) {
  send(_peers, OP_PRINT, msg->getPayload());
}


KF_EXPORT
void init() {
  Runtime::get().registerMessageFormat(HelloPeersAgent::OP_LINK, KType::GUID);
  Runtime::get().registerMessageFormat(HelloPeersAgent::OP_PRINT, KType::STRING);
}


KF_EXPORT
Agent* instantiate(k_guid_t guid, k_integer_t cloneId) {
  return new HelloPeersAgent(guid, cloneId);
}