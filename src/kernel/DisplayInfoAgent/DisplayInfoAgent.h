//
//  DisplayInfoAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/21/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__DisplayInfoAgent__
#define __AgentRuntimeEnvironment__DisplayInfoAgent__

#include <knorba/Agent.h>

using namespace knorba;
using namespace knorba::type;
using namespace knorba::protocol;

class DisplayInfoAgent : public Agent {
  
// --- FIELDS --- //
  
  private: Ptr<KRecord> _localSetup;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: DisplayInfoAgent(Runtime& rt, const k_guid_t& guid);
  

// --- METHODS --- //
  
  // Handlers //
  public: void handleOpSetupA(PPtr<Message> msg);
  public: void handleOpSetupQ(PPtr<Message> msg);
  
};

extern "C" void init(Runtime& rt);
extern "C" Agent* instantiate(Runtime& rt, const k_guid_t& guid);

#endif /* defined(__AgentRuntimeEnvironment__DisplayInfoAgent__) */