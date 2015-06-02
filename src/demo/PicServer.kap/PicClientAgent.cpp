//
//  PicClientAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/29/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>

// KnoRBA
#include <knorba/Agent.h>

// Internal
#include <knorba/protocol/ConsoleProtocolClient.h>

// Self
#include "PicClientAgent.h"

//\/ PicClientAgent::PConsole /\///////////////////////////////////////////////

PicClientAgent::PConsole::PConsole(PicClientAgent* agent)
: ConsoleProtocolClient(agent)
{
  // Nothing;
}


void PicClientAgent::PConsole::onInputReceived(PPtr<KString> input) {
  
}


//\/ PicClientAgent /\/////////////////////////////////////////////////////////

PicClientAgent::PicClientAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid),
  _pConsole(this)
{
  
}