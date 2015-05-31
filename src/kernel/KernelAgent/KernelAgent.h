//
//  KernelAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/20/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__KernelAgent__
#define __AgentRuntimeEnvironment__KernelAgent__

// KnoRBA
#include <knorba/protocol/TunnelingClient.h>
#include <knorba/protocol/UnixSocketClient.h>
#include <knorba/protocol/ConsoleProtocolClient.h>

// Internal
#include "ClusterSetupProtocol.h"

// Super
#include <knorba/Agent.h>
#include <knorba/AgentLoader.h>

namespace knorba {
namespace are {
  
  using namespace knorba::protocol;

  class KernelRuntime;
  class Bundle;
  
//\/ KernelAgent /\////////////////////////////////////////////////////////////
  
  class KernelAgent : public Agent {
    
  // --- NESTED TYPES --- //
    
    private: class PTunneling : public TunnelingClient {
      private: KernelAgent* _iAgent;
      private: KernelRuntime& _rt;
      public: PTunneling(KernelAgent* owner);
      public: void onReceive(PPtr<KRecord> msgData);
      public: void onReceiveBcast(PPtr<KRecord> msgData);
      public: void onAddRoute(const k_guid_t& remoteRuntime,
          const k_guid_t& remotePeer, const k_guid_t& tunnel);      
      public: void onConnectionLost(const k_guid_t& rt, const k_guid_t& kernel,
          const k_guid_t& tunnel);
    };
    
    private: struct AppRecord {
      k_guid_t shellKernel;
      k_guid_t console;
      k_guid_t appId;
      k_integer_t refCount;
    };
    
    
    private: class PromptThread : public Thread {
      private: KernelAgent& _owner;
      public: PromptThread(KernelAgent& owner);
      public: void run();
    };
    
    
    private: class WatchdogThread : public Thread {
      private: KernelAgent& _owner;
      public: WatchdogThread(KernelAgent& owner);
      public: void run();
    };
    
    
  // --- STATIC FIELDS --- //
    
    private: static SPtr<KRecordType> RUN_Q_T;
    private: static const k_octet_t RUN_Q_APPID   = 0;
    private: static const k_octet_t RUN_Q_N_NODES = 1;
    private: static const k_octet_t RUN_Q_CONSOLE = 2;
    private: static const k_octet_t RUN_Q_NAME    = 3;
    private: static const k_octet_t RUN_Q_MD5     = 4;
    private: static const k_octet_t RUN_Q_BUNDLE  = 5;
    
    private: static SPtr<KRecordType> RUN_A_T;
    private: static const k_octet_t RUN_A_SUCESS        = 0;
    private: static const k_octet_t RUN_A_BUNDLE_NEEDED = 1;
    private: static const k_octet_t RUN_A_APPID         = 2;
    private: static const k_octet_t RUN_A_N_NODES       = 3;
    private: static const k_octet_t RUN_A_NAME          = 4;
    
    public: static const SPtr<KString> OP_RUN_Q;
    public: static const SPtr<KString> OP_RUN_A;
    public: static const SPtr<KString> OP_BUNDLE;
    public: static const SPtr<KString> OP_SHUTDOWN;
    public: static const SPtr<KString> OP_RESTART;
    public: static const SPtr<KString> OP_STATUS_Q;
    public: static const SPtr<KString> OP_STATUS_A;
    public: static const SPtr<KString> OP_AGENT_DOWN;
    public: static const SPtr<KString> OP_RUNTIME_DOWN;
    public: static const SPtr<KString> OP_QUIT;
    public: static const SPtr<KString> R_INTERCOM;
    public: static const SPtr<KString> R_REMOTE_KERNEL;
    
    
  // --- STATIC METHODS --- //
    
    public: static PPtr<KRecordType> run_q_t();
    public: static PPtr<KRecordType> run_a_t();
    public: static PPtr<KRecordType> bunndle_t();
    private: static string makeSocketPath(int pid);

    
  // --- FIELDS --- //
    
    private: Ptr<WatchdogThread> _watchdogThread;
    private: Ptr<PromptThread> _promptThread;
    private: Ptr< Array<AppRecord> > _apps;
    private: UnixSocketClient _pUnixSocket;
    private: ClusterSetupProtocol _pSetup;
    private: PTunneling _pTunneling;
    private: ConsoleProtocolClient _pConsole;
    private: KernelRuntime& _rt;
    private: bool _quitFlag;
    
    // Kernel Control //
    private: bool _connected;
    private: k_guid_t _localKernel;
    private: Ptr<KString> _assignment;
    private: Ptr<KValue>  _assignmentArg;

    
  // --- (DE)CONSTRUCTORS --- //
    
    public: KernelAgent(KernelRuntime& rt, const k_guid_t& guid);
    
    
  // --- METHODS --- //
    
    private: void watchdog();
    private: void doTheAssignment(const k_guid_t& remotePeer);
    private: void doKernelPrintStatusAssignment(const k_guid_t& remotePeer);
    private: void doKernelRunAssignment(const k_guid_t& remotePeer);
    private: void runOnCluster(PPtr<Bundle> bundle, const k_guid_t& kernel,
        const k_guid_t& console);
    
    private: void runLocally(PPtr<Bundle> path, k_guid_t guid,
        const k_guid_t& console, k_integer_t nNodes);
    
    private: int getIndexForApp(const k_guid_t& app) const;
    private: void onRuntimeDown(const k_guid_t& vrt, const k_guid_t& kernel);
    
    public: void proxySend(const k_guid_t& target, PPtr<Message> msg);
    public: void proxySendToAll(PPtr<Message> msg);
    public: void kernelRun();
    public: void kernelPrintStatus(int pid);
    public: void kernelShutdown(int pid);
    public: void kernelRunBundle(int pid, const string& bundleName);
    public: void anounceDeadAgent(const k_guid_t& guid);
    public: void anounceDeadRuntime(const k_guid_t& guid);
    public: bool isHead() const;
    public: const k_guid_t& getConsoleGuid() const;
    
    // Message Handlers //
    public: void handleOpShutdown(PPtr<Message> msg);
    public: void handleOpRestart(PPtr<Message> msg);
    public: void handleOpStatusQ(PPtr<Message> msg);
    public: void handleOpStatusA(PPtr<Message> msg);
    public: void handleOpRunQ(PPtr<Message> msg);
    public: void handleOpRunA(PPtr<Message> msg);
    public: void handleOpRuntimeDown(PPtr<Message> msg);
    public: void handleOpQuit(PPtr<Message> msg);
    
    // Inherited from Agent //
    public: void handlePeerConnectionRequest(PPtr<KString> role, const k_guid_t& guid);
    public: void handlePeerDisconnected(PPtr<KString> role, const k_guid_t& guid);
    public: void finalize();
    
  };
  
  
//\/ KernelAgentLoader /\//////////////////////////////////////////////////////
  
  class KernelAgentLoader : public AgentLoader {
    public: KernelAgentLoader();
    public: void init(Runtime& rt);
    public: Agent* instantiate(Runtime& rt, const k_guid_t& guid);
  };
  
  
} // namespace are
} // namespace knorba

#endif /* defined(__AgentRuntimeEnvironment__KernelAgent__) */
