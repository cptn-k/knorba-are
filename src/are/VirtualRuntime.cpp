//
//  VirtualRuntime.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/20/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/Group.h>
#include <knorba/type/all.h>

// Internal
#include "AgentInstance.h"
#include "KernelRuntime.h"

// Self
#include "VirtualRuntime.h"

namespace knorba {
namespace are {
  
  using namespace std;
  
// --- (DE)CONSTRUCTORS --- //
  
  VirtualRuntime::VirtualRuntime(KernelRuntime& owner, PPtr<Path> knorbaHome,
      const k_guid_t& guid)
  : RuntimeBase(knorbaHome, guid),
    _owner(owner)
  {
    _closing = false;
    _console = KGlobalUid::zero();
    LOG << "(O) Virtual Runtime " << guid << EL;
  }
  
  
  VirtualRuntime::~VirtualRuntime() {
    LOG << "(X) Virtual Runtime " << getGuid() << EL;
  }
  
  
// --- METHODS --- //
  
  Logger::Stream& VirtualRuntime::log(const Logger::level_t level) const {
    return System::getLogger().log(level) << "VRT "
        << getAppName() << "[" << getGuid().nodeRank << "]: ";
  }
  
  
  void VirtualRuntime::deliver(const k_guid_t &receiver, PPtr<Message> msg) {
    if(!deliverLocally(receiver, msg)) {
      _owner.deliver(receiver, msg);
    }
  }
  
  
  void VirtualRuntime::shutdown() {
    _closing = true;
    RuntimeBase::shutdown();
  }
  
  
  void VirtualRuntime::setConsole(const k_guid_t& console) {
    _console = console;
  }
  

// Inherited from RuntimeBase //
  
  Ptr<AgentLoader> VirtualRuntime::createLoaderFor(PPtr<Path> kap,
      const string &className)
  {
    return _owner.createLoaderFor(kap, className);
  }
  
  
  void VirtualRuntime::sendFromAre(const k_guid_t& guid, PPtr<KString> opcode,
      PPtr<KValue> payload)
  {
    Ptr<Message> msg = _owner.newMessage();
    msg->set(-1, opcode->getHashCode(), getGuid(), payload);
    deliverLocally(guid, msg);
  }
  
  
  bool VirtualRuntime::isAlive() const {
    return RuntimeBase::isAlive() || !_closing;
  }
  
  
  void VirtualRuntime::signalQuit() {
    if(!isActive()) {
      shutdown();
      _owner.signalQuit();
    }
  }
  
  
  void VirtualRuntime::handleAgentDisconnect(const k_guid_t& agent) {
    VirtualRuntime::handleAgentDisconnect(agent);
    _owner.handleAgentDisconnect(agent);
  }
  
  
// Inherited from RuntimeBase::Runtime //
  
  const k_guid_t& VirtualRuntime::getConsoleGuid() const {
    return _console;
  }
  
  
  bool VirtualRuntime::isHead() const {
    return _owner.isHead();
  }
  
  
  void VirtualRuntime::send(const k_guid_t& sender, const k_guid_t& receiver,
      const k_longint_t opcode, PPtr<KValue> content, const k_integer_t tid)
  {
    if(_closing) {
      return;
    }

    Ptr<Message> msg = _owner.newMessage();
    msg->set(tid, opcode, sender, content);
    deliver(receiver, msg);
  }
  
  
  void VirtualRuntime::send(const k_guid_t &sender, PPtr<Group> receivers,
      const k_longint_t opcode, PPtr<KValue> content, const k_integer_t tid)
  {
    if(_closing) {
      return;
    }
    
    Ptr<Message> msg = _owner.newMessage();
    msg->set(tid, opcode, sender, content);
    
    for(int i = receivers->getCount() - 1; i >=0; i--) {
      deliver(receivers->get(i), msg);
    }
  }
  
  
  void VirtualRuntime::sendToAll(const k_guid_t& sender, const k_longint_t opcode,
      PPtr<KValue> content, const k_integer_t tid)
  {
    if(_closing) {
      return;
    }
    
    _owner.sendToAll(sender, opcode, content, tid);
  }
  
  
  void VirtualRuntime::sendToLocals(const k_guid_t& sender, const k_longint_t opcode,
      PPtr<KValue> content, const k_integer_t tid)
  {
    Ptr<Message> msg = _owner.newMessage();
    msg->set(tid, opcode, sender, content);
    deliverLocally(msg);
    _owner.deliverLocally(msg);
  }
  
} // are
} // knorba