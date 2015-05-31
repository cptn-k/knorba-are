//
//  PicServerAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/27/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__PicServerAgent__
#define __AgentRuntimeEnvironment__PicServerAgent__

using namespace knorba;
using namespace knorba::type;


class PicServerAgent : public Agent {

// --- STATIC FIELDS --- //
  
  public: static const SPtr<KString> OP_START;
  

// --- (DE)CONSTRUCTORS --- //
  
  public: PicServerAgent(Runtime& rt, const k_guid_t& guid);
  
  
// --- METHODS --- //
  
  // Handlers //
  public: void handleOpStart(PPtr<Message> msg);
  
};

//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

extern "C" void init(Runtime& rt);
extern "C" Agent* instantiate(Runtime& rt, const k_guid_t& guid);


#endif /* defined(__AgentRuntimeEnvironment__PicServerAgent__) */
