//
//  FileGridOutputAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Kay Khandan on 3/8/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__FileGridOutputAgent__
#define __AgentRuntimeEnvironment__FileGridOutputAgent__

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/type/all.h>

// Internal
#include "PixmapOutputProtocol.h"

using namespace knorba;
using namespace knorba::type;

//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

extern "C" void init(Runtime& rt);
extern "C" knorba::Agent* instantiate(Runtime& rt, const k_guid_t& guid);


//\/ FileGridOutputAgent /\////////////////////////////////////////////////////

class FileGridOutputAgent : public Agent {
  
// --- STATIC FIELDS --- //
  
  public: static SPtr<KString> OP_SET_PATH;
  
  
// --- FIELDS --- //
  
  private: Ptr<Path> _dirPath;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: FileGridOutputAgent(Runtime& rt, const k_guid_t& guid);
  
  
// --- METHODS --- //
  
  private: void write(PixmapOutputProtocol::output_format_t format,
      k_integer_t phase, PPtr<KGrid> data, ostream& os);
  
  public: void handleMessage(PPtr<knorba::Message> message);
  
};

//\////////////////////////////////////////////////////////////////////////////

#endif /* defined(__AgentRuntimeEnvironment__FileGridOutputAgent__) */
