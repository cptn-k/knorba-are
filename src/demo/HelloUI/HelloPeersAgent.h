//
//  HelloAgent.h
//  ARE
//
//  Created by Hamed KHANDAN on 10/19/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//


#ifndef ARE_DEMO_HELLOPLEXERAGENT
#define ARE_DEMO_HELLOPLEXERAGENT


#include <knorba/Agent.h>
#include <knorba/Group.h>
#include <knorba/type/all.h>

using namespace knorba;
using namespace knorba::type;


class HelloPeersAgent : public Agent {
  
// --- STATIC FIELS --- //
  
  public: const static StaticPtr<KString> OP_LINK;
  public: const static StaticPtr<KString> OP_PRINT;
  

// --- FIELDS --- //
  
  private: Ptr<Group> _peers;
  
  
// --- (DE)CONSTRUCTOR --- //
  
  public: HelloPeersAgent(const k_guid_t guid, const k_integer_t cloneId);
  public: ~HelloPeersAgent();
  
  
// --- METHODS --- //
  
  public: void opLinkHandler(Ptr<Message> msg);
  public: void opSayHandler(Ptr<Message> msg);
  
};

extern "C" void init();
extern "C" knorba::Agent* instantiate(k_guid_t guid, k_integer_t cloneId);

#endif /* defined(ARE_DEMO_HELLOPLEXERAGENT) */