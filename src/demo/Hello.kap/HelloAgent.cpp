//
//  HelloAgent.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 10/19/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/type/all.h>
#include <knorba/Runtime.h>

// Self
#include "HelloAgent.h"

using namespace std;


const SPtr<KString> HelloAgent::OP_SAY = new KString("demo.hello.say");


HelloAgent::HelloAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid),
  _pConsole(this)
{
  _pConsole.setServer(getRuntime().getConsoleGuid());
  registerHandler((handler_t)&HelloAgent::opSayHandler, OP_SAY);
}


void HelloAgent::opSayHandler(PPtr<Message> msg) {
  _pConsole.print("Hello World!");
  quit();
}


KF_EXPORT
void init(Runtime& rt) {
  rt.registerMessageFormat(HelloAgent::OP_SAY, KType::NOTHING);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new HelloAgent(rt, guid);
}