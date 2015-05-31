//
//  UiTextOutputAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/16/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__UiTextOutputAgent__
#define __AgentRuntimeEnvironment__UiTextOutputAgent__

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/are/core/DisplayInfoProtocol.h>

using namespace knorba;
using namespace knorba::type;
using namespace knorba::are::core;

//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

extern "C" void init(Runtime& rt);
extern "C" knorba::Agent* instantiate(Runtime& rt, const k_guid_t& guid);


//\/ UiTextOutputAgent /\//////////////////////////////////////////////////////

class UiTextOutputAgent : public Agent {
  
// --- FIELDS --- //
  
  private: knorba::are::core::DisplayInfoProtocol _pDisplayInfo;
  private: bool _isFullScreen;
  private: Range _windowBounds;
  private: Tuple _windowOrigin;
  private: int _pipe[2];
  private: int _childId;
  private: bool _uiReady;

// --- STATIC METHODS --- //
  
  public: static void* runJarRunner(void*);
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: UiTextOutputAgent(Runtime& rt, const k_guid_t& guid);
  public: void runJar();
  public: void handleMessage(PPtr<Message> msg);
  public: void finalize();
  
};


#endif /* defined(__AgentRuntimeEnvironment__UiTextOutputAgent__) */
