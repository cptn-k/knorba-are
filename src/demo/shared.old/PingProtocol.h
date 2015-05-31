//
//  PingProtocol.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/22/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__PingProtocol__
#define __AgentRuntimeEnvironment__PingProtocol__

// KnoRBA
#include <knorba/Group.h>
#include <knorba/type/all.h>

// Super
#include <knorba/Protocol.h>

using namespace kfoundation;
using namespace knorba;

class PingProtocol : public Protocol {
  
// --- STATIC FIELDS --- //
  
  public: static const SPtr<KString> OP_PING;
  public: static const SPtr<KString> OP_INFORM;
  private: static SPtr<KRecordType> OP_PING_A_T;
  
  
// --- STATIC METHODS --- //
  
  private: static void* runPingerLoop(void*);
  public: static PPtr<KRecordType> op_inform_t();
  public: static void init(Runtime& rt);

  
// --- FIELDS --- //
  
  private: Ptr<KLongint> _groupIdentity;
  private: Ptr<Group> _peers;
  private: bool _isLeader;
  private: bool _isAlive;
  private: bool _stopFlag;
  private: int  _condNPeers;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: PingProtocol(Agent* owner, const k_longint_t groupIdentity);
  public: ~PingProtocol();
  

// --- METHODS --- //
  
  private: void pingerLoop();
  private: void inform();
  
  public: void start(int nPeers);
  public: void stop();
  public: PPtr<Group> getPeers() const;
  public: bool isLeader() const;

  // Hnadlers //
  public: void handleOpPing(PPtr<Message> msg);
  public: void handleOpInform(PPtr<Message> msg);
  
  // Events //
  public: virtual void onNewPeerFound(const k_guid_t& guid);
  public: virtual void onSearchEnd();
  
  // Inherited from Protocol //
  public: void finalize();
  public: bool isAlive() const;
  
};

#endif /* defined(__AgentRuntimeEnvironment__PingProtocol__) */