//
//  UnixSocketAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/18/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__UnixSocketAgent__
#define __AgentRuntimeEnvironment__UnixSocketAgent__

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/AgentLoader.h>
#include <knorba/type/all.h>
#include <knorba/protocol/TunnelingServer.h>

namespace knorba {
  namespace are {
    class UnixSocketInputStream;
    class UnixSocketOutputStream;
  } // namespace are
} // namespace knorba


using namespace kfoundation;
using namespace knorba;
using namespace knorba::type;
using namespace knorba::protocol;
using namespace knorba::are;


class UnixSocketAgent : public Agent {
  
// --- NESTED TYPES --- //
  
  private: class RouteTableItem : public ManagedObject {
    public: k_guid_t kernel;
    public: Ptr<Path> path;
    public: int nRetries;
  };
  
  
  private: class PTunneling : public TunnelingServer {
    private: UnixSocketAgent* _iAgent;
    public: PTunneling(UnixSocketAgent* owner);
    public: void onSend(PPtr<KRecord> data);
    public: void onSendBcast(PPtr<KRecord> data);
  };
  
  
  private: class ReceiveThread : public Thread {
    private: UnixSocketAgent& _owner;
    public: ReceiveThread(UnixSocketAgent& owner);
    public: void run();
  };
  
  
  private: class WatchdogThread : public Thread {
    private: UnixSocketAgent& _owner;
    public: WatchdogThread(UnixSocketAgent& owner);
    public: void run();
  };

  
// --- STATIC FIELDS --- //
  
  private: static SPtr<KRecordType> HELLO_T;
  private: static const k_octet_t HELLO_T_KERNEL  = 0;
  private: static const k_octet_t HELLO_T_RUNTIME = 1;
  private: static const k_octet_t HELLO_T_ADDRESS = 2;
  
  private: static const k_octet_t CODE_HELLO;
  private: static const k_octet_t CODE_PING;
  private: static const k_octet_t CODE_TUNNEL;
  private: static const k_octet_t CODE_TUNNEL_BCAST;
  
  private: static const int WATCHDOG_N_RETRIES;
  private: static const int WATCHDOG_INTERVAL;

  
// --- FIELDS --- //
  
  private: Ptr<UnixSocketInputStream> _inputSocket;
  private: Ptr<ReceiveThread> _receiveThread;
  private: Ptr<WatchdogThread> _watchdogThread;
  private: Ptr< ManagedArray<RouteTableItem> > _routeTable;
  private: PTunneling _pTunneling;
  private: bool _stopFlag;
  
  
// --- STATIC METHODS --- //
  
  private: static PPtr<KRecordType> hello_t();
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: UnixSocketAgent(Runtime& rt, const k_guid_t& guid);
  public: ~UnixSocketAgent();
  
  
// --- METHODS --- //
  
  private: void watchdogLoop();
  private: int  checkAndRegisterClient(PPtr<KRecord> helloMsg);
  private: int  getRouteTableIndexOf(const k_guid_t& guid) const;
  
  // Receive //
  private: void receiveLoop();
  private: void receive();
  private: void receiveHello(PPtr<InputStream> input);
  private: void receivePing(PPtr<InputStream> input);
  private: void receiveTunnel(PPtr<InputStream> input, bool bcast);
  
  // Send //
  private: void sayHello(PPtr<Path> target);
  private: void tunnel(PPtr<KRecord> msg, bool bcast);
  private: void netSend(PPtr<Path> target, k_octet_t opcode, PPtr<KValue> msg,
      int routeTableIndex);
  
  // Handlers //
  private: void handleOpSetAddress(PPtr<Message> msg);
  private: void handleOpAddClient(PPtr<Message> msg);
  
  // Inherited from Agent //
  public: bool isAlive();
  public: void finalize();
    
};


//\/ UnixSocketAgentLoader /\//////////////////////////////////////////////////

extern "C" void init(Runtime& rt);
extern "C" knorba::Agent* instantiate(Runtime& rt, const k_guid_t& guid);

#endif /* defined(__AgentRuntimeEnvironment__UnixSocketAgent__) */
