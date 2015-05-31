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
  
  private: static SPtr<KEnumerationType> TASTE_T;
  private: static SPtr<KRecordType> COLOR_T;
  private: static SPtr<KRecordType> APPLE_T;
  public: static const SPtr<KString> OP_SAY;
  
  
// --- STATIC METHODS --- //

  public: static SPtr<KEnumerationType> taste_t();
  public: static SPtr<KRecordType> color_t();
  public: static SPtr<KRecordType> apple_t();
  
  
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
