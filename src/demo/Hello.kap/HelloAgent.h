//
//  HelloAgent.h
//  ARE
//
//  Created by Hamed KHANDAN on 10/19/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef ARE_DEMO_HELLOAGENT
#define ARE_DEMO_HELLOAGENT

#include <knorba/protocol/ConsoleProtocolClient.h>

using namespace knorba;
using namespace knorba::type;
using namespace knorba::protocol;

class HelloAgent : public Agent {
  
// --- STATIC FIELS --- //
  
  public: static const SPtr<KString> OP_SAY;
  
  
// --- FIELDS --- //
  
  private: ConsoleProtocolClient _pConsole;
  
  
// --- CONSTRUCTOR --- //
  
  public: HelloAgent(Runtime& rt, const k_guid_t& guid);
  
  
// --- METHODS --- //
  
  public: void opSayHandler(PPtr<Message> msg);
  
};

extern "C" void init(Runtime& rt);
extern "C" knorba::Agent* instantiate(Runtime& rt, const k_guid_t& guid);

#endif /* defined(__ARE__HelloAgent__) */
