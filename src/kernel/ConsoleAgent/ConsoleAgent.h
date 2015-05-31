//
//  ConsoleAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/3/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__ConsoleAgent__
#define __AgentRuntimeEnvironment__ConsoleAgent__

#include <knorba/Agent.h>

using namespace knorba;
using namespace knorba::protocol;

class ConsoleAgent : public Agent {
  
// --- NESTED TYPES --- //
  
  private: class RelayPath : public ManagedObject {
    public: const k_guid_t sourceRuntime;
    public: const k_guid_t targetConsole;
    public: Ptr<Group> subscribers;
    public: RelayPath(const k_guid_t& vrt, const k_guid_t& console);
  };
  
  
  private: class PromptThread : public Thread {
    private: ConsoleAgent& _owner;
    public: PromptThread(ConsoleAgent& owner);
    public: void run();
  };
  
  
// --- STATIC FIELDS --- //
  
  public: static const SPtr<KString> R_SUBSCRIBER;
  
  
// --- FIELDS --- //
  
  private: Logger _logger;
  private: bool _stopFlag;
  private: bool _isReading;
  private: string _input;
  private: Ptr<PromptThread> _promptThread;
  private: Ptr< ManagedArray<RelayPath> > _relayPaths;
  private: WINDOW* _titleBar;
  private: WINDOW* _outputWindow;
  private: WINDOW* _inputWindow;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: ConsoleAgent(Runtime& rt, const k_guid_t& guid);
  
  
// --- METHODS --- //
  
  private: void promptLoop();
  private: void printInput();
  private: int getIndexForApp(const k_guid_t& appId);
  private: int getIndexForConsole(const k_guid_t& console);
  private: void print(const char* str);
  private: void sendInput();
  private: void unsubscribe(const k_guid_t& guid);
  
  // Handlers //
  public: void handleOpStart(PPtr<Message> msg);
  public: void handleOpStop(PPtr<Message> msg);
  public: void handleOpPrint(PPtr<Message> msg);
  public: void handleOpInput(PPtr<Message> msg);
  public: void handleOpSubscribe(PPtr<Message> msg);
  public: void handleOpUnsubscribe(PPtr<Message> msg);
  public: void handleOpAddRelayPath(PPtr<Message> msg);
  public: void handleOpRemoveRelayPath(PPtr<Message> msg);
  
  // Inherited from Agent //  
  public: void finalize();
  public: bool isAlive();
  public: void handlePeerDisconnected(PPtr<KString> role, const k_guid_t& guid);
  
};

// --- GLOBAL --- //

extern "C" void init(Runtime& rt);
extern "C" Agent* instantiate(Runtime& rt, const k_guid_t& guid);

#endif /* defined(__AgentRuntimeEnvironment__ConsoleAgent__) */
