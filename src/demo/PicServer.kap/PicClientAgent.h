//
//  PicClientAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/29/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__PicClientAgent__
#define __AgentRuntimeEnvironment__PicClientAgent__

using namespace kfoundation;
using namespace knorba;
using namespace knorba::type;
using namespace knorba::protocol;


class PicClientAgent : public Agent {
  
// --- NESTED TYPES --- //
  
  private: class PConsole : public ConsoleProtocolClient {
    private: PicClientAgent* _iAgent;
    public: PConsole(PicClientAgent* agent);
    public: void onInputReceived(PPtr<KString> input);
  };
  
  
// --- FIELDS --- //
  
  private: PConsole _pConsole;
  
// --- (DE)CONSTRUCTORS --- //
  
  public: PicClientAgent(Runtime& rt, const k_guid_t& guid);
  
};

#endif /* defined(__AgentRuntimeEnvironment__PicClientAgent__) */
