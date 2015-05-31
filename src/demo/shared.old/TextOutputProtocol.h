//
//  TextOutputProtocol.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/16/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__TextOutputProtocol__
#define __AgentRuntimeEnvironment__TextOutputProtocol__

#include <knorba/Group.h>
#include <knorba/Protocol.h>
#include <knorba/type/all.h>

using namespace knorba;
using namespace knorba::type;

class TextOutputProtocol : public Protocol {
  
// --- STATIC FIELDS --- //
  
  public: static SPtr<KString> OP_PING;
  public: static SPtr<KString> OP_PUT;
  
  
// --- STATIC METHODS --- //
  
  public: static void init(Runtime& rt);
  
  
// --- FIELDS --- //
  
  private: Ptr<Group> _servers;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: TextOutputProtocol(Agent* owner);
  
  
// --- METHODS --- //
  
  public: bool connectToServers();
  public: PPtr<Group> getServers();
  public: void put(const string& text);
  
};

#endif /* defined(__AgentRuntimeEnvironment__TextOutputProtocol__) */
