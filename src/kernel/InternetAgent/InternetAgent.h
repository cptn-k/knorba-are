//
//  InternetAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/30/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__InternetAgent__
#define __AgentRuntimeEnvironment__InternetAgent__

// Super
#include <knorba/Agent.h>

using namespace knorba;
using namespace knorba::type;
using namespace knorba::protocol;

class InternetAgent : public Agent {

// --- NESTED TYPES --- //
  
  private: class RouteTableItem : public ManagedObject {
    public: k_guid_t runtime;
    public: k_guid_t kernel;
    public: Ptr<InternetOutputStream> os;
    public: int nRetries;
  };
  
  
  private: struct MessageQueueItem {
    public: k_octet_t opcode;
    public: PPtr<RouteTableItem> route;
    public: Ptr<KValue> payload;
  };
  
  
  private: class PTunneling : public TunnelingServer {
    private: InternetAgent* _iAgent;
    public: PTunneling(InternetAgent* owner);
    public: void onSend(PPtr<KRecord> data);
    public: void onSendBcast(PPtr<KRecord> data);
  };
  
  
  private: class ReceiverThread : public Thread {
    private: InternetAgent& _owner;
    public: ReceiverThread(InternetAgent& owner);
    public: void run();
  };
  
  
  private: class SenderThread : public Thread {
    private: InternetAgent& _owner;
    public: SenderThread(InternetAgent& owner);
    public: void run();
  };
  
  
  private: class WatchdogThread : public Thread {
    private: InternetAgent& _owner;
    public: WatchdogThread(InternetAgent& owner);
    public: void run();
  };
  
  
// --- STATIC FIELDS --- //
  
  private: static const k_integer_t DEFAULT_PORT = 2010;
  private: static const int MESSAGE_QUEUE_SIZE = 32;
  private: static const int WATCHDOG_N_RETRIES = 5;
  private: static const int WATCHDOG_INTERVAL = 10000;
  
  private: static SPtr<KRecordType> HELLO_T;
  private: static const int HELLO_T_KERNEL  = 0;
  private: static const int HELLO_T_RUNTIME = 1;
  private: static const int HELLO_T_ADDRESS = 2;
  
  private: static const k_octet_t CODE_HELLO;
  private: static const k_octet_t CODE_PING;
  private: static const k_octet_t CODE_TUNNEL;
  private: static const k_octet_t CODE_TUNNEL_BCAST;
  
  public: static const SPtr<KString> OP_ADD_CONNECTION;
  public: static const SPtr<KString> OP_SET_ADDRESS;
  
  
// --- STATIC METHODS --- //
  
  public: static PPtr<KRecordType> hello_t();
  
  
// --- FIELDS --- //
  
  // Protocol //
  private: PTunneling _pTunneling;
  
  // Concurrency //
  private: Ptr<SenderThread>   _senderThread;
  private: Ptr<ReceiverThread> _receiverThread;
  private: Ptr<WatchdogThread> _watchDogThread;
  private: Mutex               _messageQueueMutex;
  private: Condition           _newMessageCond;
  private: bool                _stopFlag;
  
  // Communication //
  private: Ptr<InternetInputStream> _inputSocket;
  private: Ptr< ManagedArray<RouteTableItem> > _routeTable;
  
  // Message Queue //
  private: MessageQueueItem _messageQueue[MESSAGE_QUEUE_SIZE];
  private: int _queueHead;
  private: int _queueTail;
  private: int _queueCount;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: InternetAgent(Runtime& rt, const k_guid_t& guid);
  
  
// --- METHODS --- //
  
  private: PPtr<InternetAgent::RouteTableItem>
      findRouteToKernel(const k_guid_t& guid);
  
  // Threads //
  private: void senderLoop();
  private: void receiverLoop();
  private: void watchdogLoop();
  
  // Receive //
  private: void netReceive();
  private: void receiveHello();
  private: void receivePing();
  private: void receiveTunnel(bool bcast);
  
  // Send //
  private: void netSend(MessageQueueItem& messageItem);
  private: PPtr<RouteTableItem> checkAndRegisterClient(PPtr<KRecord> data);
  private: void sayHello(PPtr<RouteTableItem> route);
  private: void tunnel(PPtr<KRecord> msg, bool bcast);
  private: void enqueue(k_octet_t opecode, PPtr<RouteTableItem> route,
      PPtr<KValue> payload);
  
  // Message Handlers //
  private: void handleOpSetHost(PPtr<Message> msg);
  private: void handleOpAddConnection(PPtr<Message> msg);
  
  // Inherited from Agent //
  public: void finalize();
  public: bool isAlive();
  
};

extern "C" void init(Runtime& rt);
extern "C" knorba::Agent* instantiate(Runtime& rt, const k_guid_t& guid);

#endif /* defined(__AgentRuntimeEnvironment__InternetAgent__) */