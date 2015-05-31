//
//  StartProtocol.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/4/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__StartProtocol__
#define __AgentRuntimeEnvironment__StartProtocol__

#include <knorba/Runtime.h>
#include <knorba/Protocol.h>
#include <knorba/type/all.h>

using namespace knorba;

class StartProtocol : public Protocol {
  
// --- STATIC FIELDS --- //
  
  public: static SPtr<KString> OP_START;
  
  
// --- STATIC METHODS --- //
  
  public: static void init(Runtime& rt);
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: StartProtocol(Agent* owner);
  
  
// --- METHODS --- //
  
  public: virtual void onStart() = 0;  
  public: bool handleMessage(PPtr<Message> msg);
  
};

#endif /* defined(__AgentRuntimeEnvironment__StartProtocol__) */
