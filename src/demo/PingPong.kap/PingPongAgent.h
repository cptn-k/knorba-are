//
//  PingPongAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/22/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__PingPongAgent__
#define __AgentRuntimeEnvironment__PingPongAgent__

// Protocol

using namespace knorba;
using namespace knorba::type;
using namespace knorba::protocol;

class PingPongAgent : public Agent {
  
// --- NESTED TYPES --- //
  
  private: class PGrouping : public GroupingProtocol {
    private: PingPongAgent* _iAgent;
    public: PGrouping(PingPongAgent* owner);
    public: void onMemberDisconnected(const k_guid_t& guid);
    public: void onAllMembersConnected();
  };
  

// --- STATIC FIELDS --- //
  
  public: static const SPtr<KString> OP_START;
  public: static const SPtr<KString> OP_PING;
  public: static const SPtr<KString> OP_FINISH;
  public: static const SPtr<KString> R_MEMBER;
  public: static const int DELAY = 500;
  
  
// --- FIELDS --- //
  
  private: ConsoleProtocolClient _pConsole;
  private: PGrouping _pGrouping;
  private: Ptr<KInteger> pingCounter;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: PingPongAgent(Runtime& rt, const k_guid_t& guid);
  public: ~PingPongAgent();
  

// --- METHODS --- //
  
  private: void pingRandom();
  
  // Handlers //
  public: void handleOpStart(PPtr<Message> msg);
  public: void handleOpPing(PPtr<Message> msg);
  public: void handleOpFinish(PPtr<Message> msg);
  
};

extern "C" void init(Runtime& rt);
extern "C" knorba::Agent* instantiate(Runtime& rt, const k_guid_t& guid);

#endif /* defined(__AgentRuntimeEnvironment__PingPongAgent__) */
