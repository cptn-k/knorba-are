//
//  ChatAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/20/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__ChatAgent__
#define __AgentRuntimeEnvironment__ChatAgent__

using namespace kfoundation;
using namespace knorba;
using namespace knorba::type;
using namespace knorba::protocol;

class ChatAgent : public Agent {
  
// --- NESTED TYPES --- //
  
  private: class PConsole : public ConsoleProtocolClient {
    public: PConsole(Agent* agent);
    public: void onInputReceived(PPtr<KString> input);
  };

  
// --- STATIC FIELDS --- //
  
  private: static SPtr<KRecordType> MESSAGE_T;
  private: static const k_octet_t MESSAGE_T_SENDER = 0;
  private: static const k_octet_t MESSAGE_T_CONTENT = 1;
  
  public: static const SPtr<KString> OP_MESSAGE;
  
  
// --- STATIC METHODS --- //
  
  public: static SPtr<KRecordType> message_t();
  
  
// --- FIELDS --- //
  
  private: PConsole _pConsole;
  private: string _nickName;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: ChatAgent(Runtime& rt, const k_guid_t& guid);
  
  
// --- METHODS --- //
  
  public: void processInput(PPtr<KString> input);
  public: void handleOpMessage(PPtr<Message> msg);
  
};

extern "C" void init(Runtime& rt);
extern "C" knorba::Agent* instantiate(Runtime& rt, const k_guid_t& guid);

#endif /* defined(__AgentRuntimeEnvironment__ChatAgent__) */
