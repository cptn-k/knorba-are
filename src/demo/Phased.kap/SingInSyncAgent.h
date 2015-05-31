//
//  SingInSyncAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/23/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__SingInSyncAgent__
#define __AgentRuntimeEnvironment__SingInSyncAgent__


using namespace knorba;
using namespace knorba::type;
using namespace knorba::protocol;

class SingInSyncAgent : public Agent {
  
// --- NESTED TYPES --- //
  
  private: class PGrouping : public GroupingProtocol {
    private: SingInSyncAgent* _iAegnt;
    public: PGrouping(SingInSyncAgent* agent);
    public: void onAllMembersConnected();
  };
  
  
  private: class SingThread : public Thread {
    private: SingInSyncAgent* _iAgent;
    public: SingThread(SingInSyncAgent* agent);
    public: void run();
  };
  
  
// --- STATIC FIELDS --- //
  
  private: static SPtr<KGridType> LYRICS_T;
  public: static const SPtr<KString> OP_SING;
  public: static const SPtr<KString> R_MATE;
  public: static const int WAIT = 3000;
  
  
// --- STATIC METHODS --- //
  
  public: static SPtr<KGridType> lyrics_t();
  
  
// --- FIELDS --- //
  
  private: Ptr<KGrid> _lyrics;
  private: Ptr<SingThread> _thread;
  private: bool _stopFlag;
  
  // Protocols //
  private: ConsoleProtocolClient _pConsole;
  private: PGrouping             _pGrouping;
  private: PhaserProtocol        _pPhaser;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: SingInSyncAgent(Runtime& rt, const k_guid_t& guid);
  
  
// --- METHODS --- //
  
  private: void singLoop();
  
  // Handlers //
  public: void handleOpSing(PPtr<Message> msg);
  
  // Inherited from Agent //
  public: bool isAlive();
  public: void finalize();
  
};


extern "C" void init(Runtime& rt);
extern "C" knorba::Agent* instantiate(Runtime& rt, const k_guid_t& guid);

#endif /* defined(__AgentRuntimeEnvironment__SingInSyncAgent__) */
