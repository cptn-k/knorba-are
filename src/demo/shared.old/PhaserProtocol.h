//
//  PhaserProtocols.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/23/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__PhaserProtocols__
#define __AgentRuntimeEnvironment__PhaserProtocols__

// KnoRBA
#include <knorba/Group.h>
#include <knorba/type/all.h>
#include <knorba/Runtime.h>

// Super
#include <knorba/Protocol.h>

using namespace kfoundation;
using namespace knorba;

class PhaserProtocol : public Protocol {
  
// --- STATIC FIELDS --- //
  
  public: static const SPtr<KString> OP_READY;
  public: static const SPtr<KString> OP_SIGNAL;
  public: static SPtr<KRecordType> SIGNAL_T;
  
// --- STATIC METHODS --- //
  
  private: static PPtr<KRecordType> signal_t();
  private: static void* runLoop(void*);
  public: static void init(Runtime& rt);

  
// --- FIELDS --- //
  
  private: Ptr<KLongint> _groupIdentity;
  private: Ptr<Group> _peers;
  private: k_integer_t _delay;
  private: k_longint_t _pahse;
  private: k_guid_t _leader;
  private: bool _isLeader;
  private: bool _isAlive;
  private: bool _stopFlag;
  private: int  _nInWait;
  private: pthread_mutex_t _condMutex;
  private: pthread_cond_t  _cond;
  
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: PhaserProtocol(Agent* owner, const k_longint_t groupId,
      const k_integer_t delay);
  
  public: ~PhaserProtocol();
  
  
// --- METHODS --- //
  
  private: void loop();
  private: void signalPeers();
  private: void signalTimer();
  protected: bool handleMessage(PPtr<Message> msg);
  
  public: void addPeer(const k_guid_t& guid);
  public: PPtr<Group> getPeers() const;
  public: bool isLeader() const;
  public: void setDelay(const k_integer_t d);
  public: k_integer_t getDelay() const;
  public: void start();
  public: void stop();
  public: k_longint_t getPhase() const;
    
  // Events //
  public: virtual void onSignal(const k_longint_t pahse);
  
  // Inherited from Protocol //
  public: void finalize();
  public: bool isAlive() const;
  
};

#endif /* defined(__AgentRuntimeEnvironment__PhaserProtocols__) */
