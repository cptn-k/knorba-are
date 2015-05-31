//
//  DisplayArrangmentProtocol.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/24/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#include <kfoundation/RangeIterator.h>
#include <knorba/MessageSet.h>

#include "DisplayArrangmentProtocol.h"


//\/ DisplayArrangmentProtocol::Peer /\////////////////////////////////////////

DisplayArrangmentProtocol::Peer::Peer(const k_guid_t& guid, const Range& r,
    const Direction& d)
: guid(guid),
  range(r),
  relativePosition(d)
{
  // Nothing;
}


void DisplayArrangmentProtocol::Peer::serialize(PPtr<ObjectSerializer> os) const
{
  os->object("Peer")
    ->attribute("guid", KGlobalUid::toString(guid))
    ->attribute("range", range.toString())
    ->attribute("direction", relativePosition.toString());
}


//\/ DisplayArrangmentProtocol /\//////////////////////////////////////////////

// --- STATIC FIELDS --- //

const SPtr<KString> DisplayArrangmentProtocol::OP_QUERY_Q
    = new KString("knorba.demo.display-arrangment.query-q");

const SPtr<KString> DisplayArrangmentProtocol::OP_QUERY_A
    = new KString("knorba.demo.display-arrangment.query-a");

const SPtr<KString> DisplayArrangmentProtocol::OP_INFORM
    = new KString("knorba.demo.display-arrangment.inform");

const SPtr<KString> DisplayArrangmentProtocol::OP_DISPLAYINFO_QUERY_Q
    = new KString("knorba.displayinfo.query-q");

SPtr<KRecordType> DisplayArrangmentProtocol::INFORM_RECORD_T;
SPtr<KGridType> DisplayArrangmentProtocol::INFORM_T;


// --- STATIC METHODS --- //

PPtr<KRecordType> DisplayArrangmentProtocol::inform_record_t() {
  if(INFORM_RECORD_T.isNull()) {
    INFORM_RECORD_T = new KRecordType("knorba.demo.display-arrangment.record");
    INFORM_RECORD_T->addField("id", KType::GUID)
                   ->addField("bounds", Rect::type().AS(KType));
  }
  return INFORM_RECORD_T;
}


PPtr<KGridType> DisplayArrangmentProtocol::inform_t() {
  if(INFORM_T.isNull()) {
    INFORM_T = new KGridType(inform_record_t(), 1);
  }
  
  return INFORM_T;
}


void DisplayArrangmentProtocol::init(Runtime &rt) {
  rt.registerMessageFormat(OP_QUERY_Q, KType::NOTHING);
  rt.registerMessageFormat(OP_QUERY_A, Rect::type().AS(KType));
  rt.registerMessageFormat(OP_INFORM, inform_t().AS(KType));
}


// --- (DE)CONSTRUCTORS --- //

DisplayArrangmentProtocol::DisplayArrangmentProtocol(Agent* owner,
    const k_longint_t groupId)
: DisplayInfoProtocol(owner, true)
{
  _isLeader = (owner->getGuid().nodeRank == 1);
  
  _groupIdentity = new KLongint(groupId);
  _peerGroup = new Group();
  _peers = new ManagedArray<Peer>();
}


DisplayArrangmentProtocol::~DisplayArrangmentProtocol() {
  // Nothing;
}


// --- METHODS --- //

void DisplayArrangmentProtocol::computeAndInform() {
  Tuple1D n(_peers->getSize());

  Ptr<KGridBasic> grid = new KGridBasic(inform_t(), n + 1);
  Ptr<KRecord> wrapper = new KRecord(grid.AS(KGrid));
  Ptr<Rect> rectWrapper = new Rect();
  wrapper->getRecord(1, rectWrapper.AS(KRecord));
  
  for(RangeIterator i(n); i.hasMore(); i.next()) {
    grid->at(i, wrapper);
    PPtr<Peer> peer = _peers->at(i.at(0));
    wrapper->setGuid(0, peer->guid);
    rectWrapper->set(peer->range);
  }
  
  grid->at(n, wrapper);
  wrapper->setGuid(0, getOwner()->getGuid());
  rectWrapper->set(getBounds());
  
  getOwner()->send(_peerGroup, OP_INFORM, grid.AS(KValue));
}


const Range& DisplayArrangmentProtocol::getGlobalBounds() const {
  return _globalBounds;
}


PPtr<Group> DisplayArrangmentProtocol::getPeerGroup() const {
  return _peerGroup;
}


PPtr< ManagedArray<DisplayArrangmentProtocol::Peer> >
DisplayArrangmentProtocol::getPeers() const
{
  return _peers;
}


void DisplayArrangmentProtocol::inititate(PPtr<Group> peers) {
  _peerGroup = peers;
  queryLocalDisplaySetup();
  _globalBounds = getBounds();
  
  if(peers->isEmpty()) {
    onSetupComplete();
  } else {
    if(_isLeader) {
      _nWaitingFor = peers->getCount();
      getOwner()->send(peers, OP_QUERY_Q, KValue::NOTHING);
    }
  }
}


// Handlers //

bool DisplayArrangmentProtocol::handleMessage(PPtr<Message> msg) {
  if(msg->is(OP_QUERY_A)) {
    handleOpQueryA(msg);
    return true;
  } else if(msg->is(OP_QUERY_Q)) {
    handleOpQueryQ(msg);
    return true;
  } else if(msg->is(OP_INFORM)) {
    handleOpInform(msg);
    return true;
  }
  
  return false;
}


void DisplayArrangmentProtocol::handleOpQueryQ(PPtr<Message> msg) {
  getOwner()->log() << "Received OP_QUERY_Q" << EL;
  
  queryLocalDisplaySetup();
  _globalBounds = getBounds();
  getOwner()->respond(msg, OP_QUERY_A, new Rect(getBounds()));
}


void DisplayArrangmentProtocol::handleOpQueryA(PPtr<Message> msg) {
  getOwner()->log() << "Received OP_QUERY_A: " << *msg->getPayload() << EL;
  
  if(!_isLeader) {
    return;
  }

  Ptr<KRecord> data = msg->getPayload().AS(KRecord);
  
  Ptr<Rect> rect = new Rect();
  rect->KRecord::set(msg->getPayload().AS(KValue));
  
  Range r = rect->get();
  Direction d = r.getRelativePositionTo(getBounds());
  _globalBounds = _globalBounds.joinWith(r);
  
  _peers->push(new Peer(msg->getSender(), r, d));
  
  _nWaitingFor--;
  if(_nWaitingFor == 0) {
    computeAndInform();
    onSetupComplete();
  }
}


void DisplayArrangmentProtocol::handleOpInform(PPtr<Message> msg) {
  PPtr<KGridBasic> items = msg->getPayload().AS(KGridBasic);
  Ptr<KRecord> wrapper = new KRecord(items.AS(KGrid));
  Ptr<Rect> rect = new Rect();
  wrapper->getRecord(1, rect.AS(KRecord));
  
  for(RangeIterator i(items->getSize()); i.hasMore(); i.next()) {
    items->at(i, wrapper);
    
    k_guid_t peerGuid = wrapper->getGuid(0);
    if(KGlobalUid::areEqual(peerGuid, getOwner()->getGuid())) {
      continue;
    }
    
    Range r = rect->get();
    Direction d = r.getRelativePositionTo(getBounds());
    
    if(d.isCenter()) {
      getOwner()->log(Logger::WRN) << "Peer " << peerGuid << " is located at"
          " center. Range: " << r << ", Direction: " << d << EL_CON;
      continue;
    }
    
    _globalBounds = _globalBounds.joinWith(r);
    
    _peers->push(new Peer(peerGuid, r, d));
  }
  
  onSetupComplete();
}


// Events //

void DisplayArrangmentProtocol::onSetupComplete() {
  // Nothing;
}


// Inherited from Protocol //

bool DisplayArrangmentProtocol::isAlive() const {
  return false;
}

