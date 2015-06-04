//
//  PicServerAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/27/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__PicServerAgent__
#define __AgentRuntimeEnvironment__PicServerAgent__

using namespace knorba;
using namespace knorba::type;
using namespace knorba::protocol;
using namespace knorba::demo;

class PicServerAgent : public Agent {

// --- NESTED TYPES --- //
  
  private: class PGrouping : public GroupingProtocol {
    private: PicServerAgent* _iAgent;
    public: PGrouping(PicServerAgent* owner);
    public: void onAllMembersConnected();
  };
  
  
  private: class JarRunnerThread : public Thread {
    private: PicServerAgent* _iAgent;
    public: JarRunnerThread(PicServerAgent* agent);
    public: void run();
  };

  
// --- STATIC FIELDS --- //
  
  private: static const int READ_END = 0;
  private: static const int WRITE_END = 1;
  public: static const SPtr<KString> OP_START;
  public: static const SPtr<KString> R_MATE;
  
  
// --- STATIC METHODS --- //
  
  static char* cstr(const string& str) {
    char* buffer = new char[str.length() + 1];
    memcpy(buffer, str.c_str(), str.length() + 1);
    return buffer;
  }
  

// --- FIELDS --- //
  
  private: PGrouping _pGrouping;
  private: DisplayInfoProtocol _pDisplayInfo;
  private: PhaserProtocol _pPhaser;
  private: Ptr<JarRunnerThread> _jarRunnerThread;
  private: int _pipe[2];
  private: int _childId;
  private: bool _uiReady;
  private: bool _isWindowSet;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: PicServerAgent(Runtime& rt, const k_guid_t& guid);
  
  
// --- METHODS --- //
  
  private: void runJar();
  private: void writeToPipe(const string& str);
  
  // Handlers //
  public: void handleOpStart(PPtr<Message> msg);
  public: void handleOpLoad(PPtr<Message> msg);
  public: void handleOpPut(PPtr<Message> msg);
  public: void handleOpUnput(PPtr<Message> msg);
  public: void handleOpQuit(PPtr<Message> msg);
  
  // Inherited from Agent //
  public: void finalize();
  public: bool isAlive();
  
};

//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

extern "C" void init(Runtime& rt);
extern "C" Agent* instantiate(Runtime& rt, const k_guid_t& guid);


#endif /* defined(__AgentRuntimeEnvironment__PicServerAgent__) */
