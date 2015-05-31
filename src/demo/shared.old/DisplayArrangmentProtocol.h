//
//  DisplayArrangmentProtocol.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/24/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__DisplayArrangmentProtocol__
#define __AgentRuntimeEnvironment__DisplayArrangmentProtocol__

// KFoundation
#include <kfoundation/Range.h>
#include <kfoundation/Direction.h>

// KnoRBA
#include <knorba/Protocol.h>
#include <knorba/Agent.h>
#include <knorba/type/all.h>
#include <knorba/are/core/DisplayInfoProtocol.h>

// Internal
#include "Rect.h"

using namespace knorba;
using namespace knorba::type;
using namespace knorba::are::core;

class DisplayArrangmentProtocol : public  DisplayInfoProtocol {

// --- NESTED TYPES --- //
  
  public: class Peer : public ManagedObject, public SerializingStreamer {
    public: const k_guid_t guid;
    public: const Range range;
    public: const Direction relativePosition;
    public: Peer(const k_guid_t&, const Range&, const Direction&);
    public: void serialize(PPtr<ObjectSerializer> os) const;
  };
  
  
// --- STATIC FIELDS --- //
  
  public: static const SPtr<KString> OP_QUERY_Q;
  public: static const SPtr<KString> OP_QUERY_A;
  public: static const SPtr<KString> OP_INFORM;
  public: static const SPtr<KString> OP_DISPLAYINFO_QUERY_Q;
  
  private: static SPtr<KRecordType> INFORM_RECORD_T;
  private: static SPtr<KGridType> INFORM_T;
  

// --- STATIC METHODS --- //
  
  public: static PPtr<KRecordType> inform_record_t();
  public: static PPtr<KGridType> inform_t();
  public: static void init(Runtime& rt);
  
  
// --- FIELDS --- //
  
  private: bool          _isLeader;
  private: int           _nWaitingFor;
  private: Range         _globalBounds;
  private: Ptr<KLongint> _groupIdentity;
  private: Ptr<Group>    _peerGroup;
  private: Ptr< ManagedArray<Peer> > _peers;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: DisplayArrangmentProtocol(Agent* owner, const k_longint_t groupId);
  public: ~DisplayArrangmentProtocol();
  
  
// --- METHODS --- //
  
  private: void computeAndInform();
  public : const Range& getGlobalBounds() const;
  public : PPtr<Group> getPeerGroup() const;
  public : PPtr< ManagedArray<Peer> > getPeers() const;
  public : void inititate(PPtr<Group> peers);
  
  // Handlers //
  protected: bool handleMessage(PPtr<Message> msg);
  public: void handleOpQueryQ(PPtr<Message> msg);
  public: void handleOpQueryA(PPtr<Message> msg);
  public: void handleOpInform(PPtr<Message> msg);

  // Events //
  public: virtual void onSetupComplete();
  
  // Inherited from Protocol
  public: bool isAlive() const;
  
};

#endif /* defined(__AgentRuntimeEnvironment__DisplayArrangmentProtocol__) */
