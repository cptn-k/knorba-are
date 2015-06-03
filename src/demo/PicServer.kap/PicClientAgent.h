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
using namespace knorba::demo;


class PicClientAgent : public Agent {
  
// --- NESTED TYPES --- //
  
  private: class PConsole : public ConsoleProtocolClient {
    private: PicClientAgent* _iAgent;
    public: PConsole(PicClientAgent* agent);
    public: void onInputReceived(PPtr<KString> input);
    private: void load(PPtr<PredictiveParserBase> parser, k_integer_t ref);
    private: void put(PPtr<PredictiveParserBase> parser, k_integer_t ref);
    private: void unput(PPtr<PredictiveParserBase> parser, k_integer_t ref);
    private: void help();
  };

  
// --- STATIC FIELDS --- //
  
  public: static const SPtr<KString> R_SERVER;
  
  
// --- FIELDS --- //
  
  private: PConsole _pConsole;
  private: k_guid_t _server;

  
// --- (DE)CONSTRUCTORS --- //
  
  public: PicClientAgent(Runtime& rt, const k_guid_t& guid);
  

// --- METHODS --- //
  
  public: void handlePeerConnectionRequest(PPtr<KString> role, const k_guid_t& guid);
  
};

//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

extern "C" void init(Runtime& rt);
extern "C" Agent* instantiate(Runtime& rt, const k_guid_t& guid);

#endif /* defined(__AgentRuntimeEnvironment__PicClientAgent__) */
