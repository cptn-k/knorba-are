//
//  PhaserProtocols.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/23/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KnoRBA
#include <knorba/Agent.h>

// Self
#include "PhaserProtocol.h"

#define MEGA 1000000

// --- STATIC FIELDS --- //

const SPtr<KString> PhaserProtocol::OP_READY
    = new KString("knorba.phaser.ready");

const SPtr<KString> PhaserProtocol::OP_SIGNAL
    = new KString("knorba.pahser.schedule-next");

SPtr<KRecordType> PhaserProtocol::SIGNAL_T;

// --- STATIC METHODS --- /

PPtr<KRecordType> PhaserProtocol::signal_t() {
  if(SIGNAL_T.isNull()) {
    SIGNAL_T = new KRecordType(OP_SIGNAL->toUtf8String());
    SIGNAL_T->addField("phase", KType::LONGINT)
              ->addField("time", KType::LONGINT);
  }
  return SIGNAL_T;
}


void* PhaserProtocol::runLoop(void* arg) {
  ((PhaserProtocol*)arg)->loop();
  return NULL;
}


void PhaserProtocol::init(Runtime &rt) {
  rt.registerMessageFormat(OP_READY, KType::LONGINT);
  rt.registerMessageFormat(OP_SIGNAL, signal_t().AS(KType));
}


// --- (DE)CONSTRUCTORS --- //

PhaserProtocol::PhaserProtocol(Agent* owner, const k_longint_t groupId,
    const k_integer_t delay)
: Protocol(owner)
{
  _isAlive       = false;
  _isLeader      = (owner->getGuid().nodeRank == 1);
  _groupIdentity = new KLongint(groupId);
  _peers         = new Group();
  _stopFlag      = false;
  _delay         = delay;
}


PhaserProtocol::~PhaserProtocol() {
  // Nothing;
}


// --- METHODS --- //

void PhaserProtocol::loop() {
  _isAlive = true;
  
  while (!_stopFlag) {
    while (_nInWait > 0 && !_stopFlag) {
//      pthread_mutex_lock(&_condMutex);
//      pthread_cond_wait(&_cond, &_condMutex);
//      pthread_mutex_unlock(&_condMutex);
      System::usleep(1000);
    }
    
    if(_stopFlag) {
      break;
    }
    
    if(_delay > 0) {
      System::usleep(_delay * 1000);
    }
    
    _pahse++;
    signalPeers();
    onSignal(_pahse);
  }
  
  _isAlive = false;
}


void PhaserProtocol::signalPeers() {
  if(_stopFlag) {
    return;
  }
  
  Ptr<KRecord> signal = new KRecord(signal_t());
  signal->setLongint(0, _pahse);
  if(!_peers->isEmpty()) {
    getOwner()->send(_peers, OP_SIGNAL, signal.AS(KValue));
  }
  
  _nInWait = _peers->getCount();
}


void PhaserProtocol::signalTimer() {
//  pthread_mutex_lock(&_condMutex);
//  pthread_cond_signal(&_cond);
//  pthread_mutex_unlock(&_condMutex);
}


bool PhaserProtocol::handleMessage(PPtr<Message> msg) {
  if(msg->is(OP_READY)) {
    if(_isLeader) {
      _nInWait--;
      signalTimer();
    } else {
      getOwner()->respond(msg, OP_READY, _groupIdentity.AS(KValue));
    }
    
    return true;
  }
  
  if(msg->is(OP_SIGNAL)) {
    if(_isLeader) {
      LOG_ERR << "PHASER: Leader received signal!" << EL;
    } else {
      _pahse = msg->getPayload().AS(KRecord)->getLongint(0);
      onSignal(_pahse);
      getOwner()->respond(msg, OP_READY, _groupIdentity.AS(KValue));
    }
    
    return true;
  }
  
  return false;
}


void PhaserProtocol::addPeer(const k_guid_t& guid) {
  _peers->add(guid);
}


PPtr<Group> PhaserProtocol::getPeers() const {
  return _peers;
}


bool PhaserProtocol::isLeader() const {
  return _isLeader;
}


k_integer_t PhaserProtocol::getDelay() const {
  return _delay;
}


void PhaserProtocol::setDelay(const k_integer_t d) {
  _delay = d;
}


void PhaserProtocol::start() {
  _pahse = 0;
  _stopFlag = false;
  if(_isLeader) {
    System::makeDetachedThread(&PhaserProtocol::runLoop, this);
  }
}


void PhaserProtocol::stop() {
  _stopFlag = true;
  if(_isLeader) {
    signalTimer();
  }
}


k_longint_t PhaserProtocol::getPhase() const {
  return _pahse;
}


// Events //

void PhaserProtocol::onSignal(k_longint_t pahse) {
  // Nothing;
}


// Inherited from Protocol //

void PhaserProtocol::finalize() {
  stop();
}


bool PhaserProtocol::isAlive() const {
  return _isAlive;
}

