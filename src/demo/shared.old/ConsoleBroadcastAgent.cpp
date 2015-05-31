//
//  ConsoleBroadcastAgent.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 10/20/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#include <iostream>
#include <knorba/Runtime.h>

#include "ConsoleBroadcastAgent.h"


using namespace std;


const StaticPtr<KString> ConsoleBroadcastAgent::OP_INIT(new KString("knorba.demo.consolebroadcastagent.init"));
const StaticPtr<KString> ConsoleBroadcastAgent::OP_PRINT(new KString("knorba.demo.text.print"));


ConsoleBroadcastAgent::ConsoleBroadcastAgent(const k_guid_t& guid, const k_integer_t cloneRank)
: Agent(guid, cloneRank)
{
  // Nothing
}


void ConsoleBroadcastAgent::handleMessage(Ptr<Message> msg) {
  if(msg->is(OP_INIT)) {
    
    string str;
    
    while(true) {
      cin >> str;
      
      if(str == "quit") {
        quit();
        return;
      }
      
      sendToAll(OP_PRINT, new KString(str));
    }
    
  } // if(msg->is(OP_INIT))
}


KF_EXPORT
void init() {
  Runtime::get().registerMessageFormat(ConsoleBroadcastAgent::OP_PRINT, KType::STRING);
}


KF_EXPORT
Agent* instantiate(k_guid_t guid, k_integer_t cloneId) {
  return new ConsoleBroadcastAgent(guid, cloneId);
}
