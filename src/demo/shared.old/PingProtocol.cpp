//
//  PingProtocol.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/22/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/RangeIterator.h>

// KnoRBA
#include <knorba/Agent.h>

// Self
#include "PingProtocol.h"

// --- STATIC FIELDS --- //

const SPtr<KString> PingProtocol::OP_PING
    = new KString("knorba.ping.ping");

const SPtr<KString> PingProtocol::OP_INFORM
    = new KString("knorba.ping.inform");

SPtr<KRecordType> PingProtocol::OP_PING_A_T;


// --- STATIC METHODS --- //

void* PingProtocol::runPingerLoop(void* arg) {
  ((PingProtocol*)arg)->pingerLoop();
  return NULL;
}


PPtr<KRecordType> PingProtocol::op_inform_t() {
  if(OP_PING_A_T.isNull()) {
    Ptr<KRecordType> rt = new KRecordType(KType::GUID);
    Ptr<KGridType> gt = new KGridType(rt, 1);
    OP_PING_A_T = new KRecordType(OP_INFORM->toUtf8String());
    OP_PING_A_T->addField("groupId", KType::LONGINT)
               ->addField("members", gt.AS(KType));
  }
  return OP_PING_A_T;
}


void PingProtocol::init(Runtime& rt) {
  rt.registerMessageFormat(OP_PING, KType::LONGINT);
  rt.registerMessageFormat(OP_INFORM, op_inform_t().AS(KType));
}


// --- (DE)CONSTRUCTORS --- //

PingProtocol::PingProtocol(Agent* owner, const k_longint_t groupIdentity)
: Protocol(owner)
{
  _isAlive = false;
  _isLeader = (owner->getGuid().nodeRank == 1);
  _groupIdentity = new KLongint(groupIdentity);
  _peers = new Group();
  registerHandler(OP_PING, (phandler_t)&PingProtocol::handleOpPing);
  registerHandler(OP_INFORM, (phandler_t)&PingProtocol::handleOpInform);
}


PingProtocol::~PingProtocol() {
  // Nothing
}


// --- METHODS --- //

void PingProtocol::pingerLoop() {
  _isAlive = true;
  
  while(!_stopFlag) {
    getOwner()->sendToAll(OP_PING, _groupIdentity.AS(KValue));
    System::usleep(1000000);
  }
  
  _isAlive = false;
}


void PingProtocol::inform() {
  Ptr<KRecord> r = new KRecord(op_inform_t());
  
  r->setLongint(0, _groupIdentity->get());
  PPtr<KGrid> g = r->field<KGrid>(1);
  
  Tuple1D n(_peers->getCount());
  g->resetWithSize(n + 1);
  
  Ptr<KRecord> wrapper = new KRecord(g);
  for(RangeIterator it(n); it.hasMore(); it.next()) {
    g->at(it, wrapper)->setGuid(_peers->get(it.at(0)).guid);
  }
  
  g->at(n, wrapper)->setGuid(getOwner()->getGuid());
  
  getOwner()->send(_peers, OP_INFORM, r.AS(KValue));
}


void PingProtocol::start(int nPeers) {
  _condNPeers = nPeers - 1;
  
  if(_condNPeers == 0) {
    onSearchEnd();
    return;
  }
  
  if(!_isLeader) {
    return;
  }
  
  _stopFlag = false;
  System::makeDetachedThread(&PingProtocol::runPingerLoop, this);
}


void PingProtocol::stop() {
  _stopFlag = true;
  while(_isAlive) {
    System::usleep(100000);
  }
}


PPtr<Group> PingProtocol::getPeers() const {
  return _peers;
}


bool PingProtocol::isLeader() const {
  return _isLeader;
}


// Handlers //

void PingProtocol::handleOpPing(PPtr<Message> msg) {
  if(msg->getPayload().AS(KLongint)->get() != _groupIdentity->get()) {
    return;
  }
  
  if(!_isLeader) {
    getOwner()->respond(msg, OP_PING, _groupIdentity.AS(KValue));
  } else if(_isAlive) {
    const k_guid_t& sender = msg->getSender();
    if(!_peers->containts(sender)) {
      _peers->add(sender);
      onNewPeerFound(sender);
      if(_condNPeers > 0 && _peers->getCount() == _condNPeers) {
        stop();
        inform();
        onSearchEnd();
      }
    }
  }
} // handleOpPing()


void PingProtocol::handleOpInform(PPtr<Message> msg) {
  PPtr<KRecord> r = msg->getPayload().AS(KRecord);
  if(r->getLongint(0) != _groupIdentity->get()) {
    return;
  }
  
  PPtr<KGrid> g = r->field<KGrid>(1);
  Ptr<KRecord> wrapper = new KRecord(g);
  for(RangeIterator i(g->getSize()); i.hasMore(); i.next()) {
    k_guid_t guid = g->at(i, wrapper)->getGuid();
    if(!KGlobalUid::areEqual(guid, getOwner()->getGuid())) {
      _peers->add(guid);
      onNewPeerFound(guid);
    }
  }
  onSearchEnd();  
}


// Events //

void PingProtocol::onNewPeerFound(const k_guid_t& guid) {
  // Nothing;
}


void PingProtocol::onSearchEnd() {
  // Nothing;
}


// Inherited from Protocol

void PingProtocol::finalize() {
  stop();
  Protocol::finalize();
}


bool PingProtocol::isAlive() const {
  return _isAlive || Protocol::isAlive();
}
