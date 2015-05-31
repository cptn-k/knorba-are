//
//  ConsoleBroadcastAgent.h
//  ARE
//
//  Created by Hamed KHANDAN on 10/20/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef ARE_DEMO_CONSOLEBROADCASTAGENT
#define ARE_DEMO_CONSOLEBROADCASTAGENT


#include <knorba/Agent.h>
#include <knorba/type/all.h>

using namespace knorba;
using namespace knorba::type;


class ConsoleBroadcastAgent : public Agent {

// --- STATIC FIELDS --- //
  
  public: const static StaticPtr<KString> OP_INIT;
  public: const static StaticPtr<KString> OP_PRINT;
  
  
// --- CONSTRUCTOR --- //
  
  public: ConsoleBroadcastAgent(const k_guid_t& guid, const k_integer_t cloneRank);
  
  
// --- METHODS --- //
  
  // Inherited from Agent
  public: void handleMessage(Ptr<Message> msg);
  
};


extern "C" void init();
extern "C" knorba::Agent* instantiate(k_guid_t guid, k_integer_t cloneId);


#endif /* defined(ARE_DEMO_CONSOLEBROADCASTAGENT) */
