//
//  KernelRuntime.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/20/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/Path.h>
#include <kfoundation/ObjectPoolMemoryManager.h>
#include <kfoundation/FileOutputStream.h>
#include <kfoundation/FileInputStream.h>
#include <kfoundation/ObjectSerializer.h>

// KnoRBA
#include <knorba/Message.h>
#include <knorba/Agent.h>
#include <knorba/Group.h>
#include <knorba/AgentLoader.h>
#include <knorba/type/all.h>

// Internal
#include "../kernel/KernelAgent/KernelAgent.h"
#include "AgentInstance.h"
#include "VirtualRuntime.h"
#include "DynamicAgentLoader.h"

// Self
#include "KernelRuntime.h"

#define KNORBA_VERSION "0.5"

namespace knorba {
namespace are {
  
// --- STATIC FIELDS --- //
  
  SPtr<KRecordType> KernelRuntime::PID_FILE_T;
  
  
// --- STATIC METHODS --- //
  
  PPtr<KRecordType> KernelRuntime::pid_file_t() {
    if(PID_FILE_T.isNull()) {
      PID_FILE_T = new KRecordType("core.are.pid-file");
      PID_FILE_T->addField("pid", KType::INTEGER)
          ->addField("guid", KType::GUID);
    }
    return PID_FILE_T;
  }
  
  
  k_guid_t KernelRuntime::generateRandomGuid() {
    k_guid_t t = KGlobalUid::zero();
    KGlobalUid::randomizeAppId(t);
    return t;
  }
  
  
// --- (DE)CONSTRUCTORS --- //
  
  /**
   * Primary constructor.
   */
  KernelRuntime::KernelRuntime(PPtr<Path> knorbaHome)
  : RuntimeBase(knorbaHome, generateRandomGuid()),
    _messagePool(64, 2),
    _closing(false)
  {
    LOG << "(O) Kernel Runtime " << getGuid() << EL;
    
    Ptr<Path> tmpDirPath = getHome()->addSegement("data");
    if(!tmpDirPath->exists()) {
      tmpDirPath->makeDir();
    }
    
    Ptr<Path> pidFilePath = tmpDirPath->addSegement("are.pid");
    
    _pidFile = new FileOutputStream(pidFilePath);
    
    Ptr<KernelAgentLoader> loader = new KernelAgentLoader();
    loader->init(*this);
    PPtr<AgentInstance> kernelAgentInstance
        = instantiate(loader.AS(AgentLoader), "kernel");
    _kernelAgent = (KernelAgent*)kernelAgentInstance->getObject();
    
    _virtualRuntimes = new ManagedArray<VirtualRuntime>();
  }
  
  
  KernelRuntime::~KernelRuntime() {
    LOG << "(X) Kernel Runtime " << getGuid() << EL;
  }
  
  
// --- METHODS --- //
  
  Ptr<KRecord> KernelRuntime::getPidFileContents() const {
    if(!_pidFile->getPath()->exists()) {
      return NULL;
    }
    
    if(!_pidFile->isLocked()) {
      return NULL;
    }
    
    Ptr<FileInputStream> fis = new FileInputStream(_pidFile->getPath());
    
    Ptr<KRecord> fileContents = new KRecord(pid_file_t());
    fileContents->readFromBinaryStream(fis.AS(InputStream));
    
    return fileContents.retain();
  }
  
  
  void KernelRuntime::makePidFile() const {
    _pidFile->lock();
    _pidFile->truncate();
    Ptr<KRecord> fileContents = new KRecord(pid_file_t());
    fileContents->setInteger(0, System::getPid());
    fileContents->setGuid(1, getGuid());
    fileContents->writeToBinaryStream(_pidFile.AS(OutputStream));
  }
  
  
  void KernelRuntime::releasePidFile() const {
    Ptr<Path> pidFilePath = getHome()->addSegement("tmp/are.pid");
    if(!pidFilePath->exists()) {
      return;
    }
    
    Ptr<FileOutputStream> fos = new FileOutputStream(pidFilePath);
    if(!fos->isLocked()) {
      fos->unlock();
    }
    fos->close();
    
    pidFilePath->remove();
  }
  
  
  void KernelRuntime::kernelRun() {
    _kernelAgent->kernelRun();
  }
  
  
  void KernelRuntime::kernelShutdown(int pid) {
    _kernelAgent->kernelShutdown(pid);
  }
  
  
  void KernelRuntime::kernelRestart(int pid) {
    LOG_ERR << "restart(int)" << EL;
  }
  
  
  void KernelRuntime::kernelPrintStatus(int pid) {
    _kernelAgent->kernelPrintStatus(pid);
  }
  
  
  void KernelRuntime::kernelRunBundle(int pid, const string& bundle) {
    _kernelAgent->kernelRunBundle(pid, bundle);
  }
  
  
  void KernelRuntime::runBundleInMainThread(PPtr<VirtualRuntime> rt, PPtr<Bundle> b) {
    _runParam = new RunParam();
    _runParam->rt = rt;
    _runParam->bundle = b;
    signalQuit();
  }

  
  PPtr<VirtualRuntime> KernelRuntime::newVirtualRuntime(const k_guid_t& guid) {
    Ptr<VirtualRuntime> vrt = new VirtualRuntime(*this, getHome(), guid);
    _virtualRuntimes->push(vrt);
    return vrt;
  }
  
  
  Ptr<Message> KernelRuntime::newMessage() {
    return _messagePool.get();
  }
  
  
  void KernelRuntime::deliver(const k_guid_t& receiver, PPtr<Message> msg) {
    if(!deliverLocally(receiver, msg)) {
      if(!deliverToVirtualRuntimes(receiver, msg)) {
        _kernelAgent->proxySend(receiver, msg);
      }
    }
  }
  
  
  bool KernelRuntime::deliverToVirtualRuntimes(const k_guid_t &receiver,
    PPtr<Message> msg)
  {
    for(int i = _virtualRuntimes->getSize() - 1; i >= 0; i--) {
      if(_virtualRuntimes->at(i)->deliverLocally(receiver, msg)) {
        return true;
      }
    }
    return false;
  }
  
  
  void KernelRuntime::deliverToAll(PPtr<Message> msg) {
    deliverLocally(msg);
    for(int i = _virtualRuntimes->getSize() - 1; i >= 0; i--) {
      _virtualRuntimes->at(i)->deliverLocally(msg);
    }
  }
  
  
// Inherited from RuntimeBase //
  
  Ptr<AgentLoader> KernelRuntime::createLoaderFor(PPtr<Path> kap,
      const string& className)
  {
    string appName = kap->getFileName();
    
    for(int i = _virtualRuntimes->getSize() - 1; i >= 0; i--) {
      if(_virtualRuntimes->at(i)->getAppName() == appName) {
        PPtr<AgentLoader> loader
            = _virtualRuntimes->at(i)->getLoaderForClass(className);
        
        if(!loader.isNull()) {
          LOG << "( ) Agent image reused: " << className << EL;
          return loader.retain();
        }
      }
    }
    
    return new DynamicAgentLoader(kap, className);
  }
  
  
  void KernelRuntime::sendFromAre(const k_guid_t& guid, PPtr<KString> opcode,
      PPtr<KValue> payload)
  {
    Ptr<Message> msg = _messagePool.get();
    msg->set(-1, opcode->getHashCode(), getGuid(), payload);
    deliver(guid, msg);
  }

  
  bool KernelRuntime::isAlive() const {
    if(RuntimeBase::isAlive()) {
      return true;
    }
    
    for(int i = _virtualRuntimes->getSize() - 1; i >= 0; i--) {
      if(_virtualRuntimes->at(i)->isAlive()) {
        return true;
      }
    }
    
    return false;
  }
  
  
  bool KernelRuntime::isActive() const {
    if(RuntimeBase::isActive()) {
      return true;
    }
    
    for(int i = _virtualRuntimes->getSize() - 1; i >= 0; i--) {
      if(_virtualRuntimes->at(i)->isActive()) {
        return true;
      }
    }
    
    return false;
  }
  
  
  void KernelRuntime::cleanup() {
    if(!_runParam.isNull()) {
      cout << Thread::getNameOfCurrentThread() << endl;
      try {
        _runParam->rt->runBundle(_runParam->bundle);
      } catch (KFException& e) {
        LOG_ERR << e.getMessage() << EL;
        LOG << e << EL;
      }
      _runParam = NULL;
      return;
    }
    
    for(int i = _virtualRuntimes->getSize() - 1; i >= 0; i--) {
      if(!_virtualRuntimes->at(i)->isAlive()) {
        k_guid_t guid = _virtualRuntimes->at(i)->getGuid();
        _virtualRuntimes->remove(i);
        if(!_closing) {
          _kernelAgent->anounceDeadRuntime(guid);
          handleRuntimeDisconnect(guid);
        }
      }
    }
    
    RuntimeBase::cleanup();
  }

  
  void KernelRuntime::shutdown() {
    LOG << "Shutting down ... " << EL;
    _closing = true;
    for(int i = _virtualRuntimes->getSize() - 1; i >= 0; i--) {
      _virtualRuntimes->at(i)->shutdown();
    }
    RuntimeBase::shutdown();
  }
  
  
  void KernelRuntime::release() {
    _closing = true;
    for(int i = _virtualRuntimes->getSize() - 1; i >= 0; i--) {
      _virtualRuntimes->at(i)->release();
    }
    RuntimeBase::release();
  }
  
  
  void KernelRuntime::handleRuntimeDisconnect(const k_guid_t& rt) {
    RuntimeBase::handleRuntimeDisconnect(rt);
    for(int i = _virtualRuntimes->getSize() - 1; i >= 0; i--) {
      _virtualRuntimes->at(i)->handleRuntimeDisconnect(rt);
    }
  }
  
  
  void KernelRuntime::handleAgentDisconnect(const k_guid_t& agent) {
    RuntimeBase::handleAgentDisconnect(agent);
    for(int i = _virtualRuntimes->getSize() - 1; i >= 0; i--) {
      if(!KGlobalUid::areOnTheSameApp(_virtualRuntimes->at(i)->getGuid(), agent)) {
        _virtualRuntimes->at(i)->handleAgentDisconnect(agent);
      }
    }
  }
  
  
  PPtr<KType> KernelRuntime::getTypeByHash(const k_longint_t hash) const {
    PPtr<KType> t = RuntimeBase::getTypeByHash(hash);
    if(t.isNull()) {
      for(int i = _virtualRuntimes->getSize() - 1; i >= 0; i--) {
        t = _virtualRuntimes->at(i)->getTypeByHash(hash);
        if(!t.isNull()) {
          return t;
        }
      }
    }
    return t;
  }
  
  
  PPtr<KType> KernelRuntime::getMessageFormatByHash(const k_longint_t hash)
  const
  {
    PPtr<KType> t = RuntimeBase::getMessageFormatByHash(hash);
    if(t.isNull()) {
      for(int i = _virtualRuntimes->getSize() - 1; i >= 0; i--) {
        t = _virtualRuntimes->at(i)->getMessageFormatByHash(hash);
        if(!t.isNull()) {
          return t;
        }
      }
    }
    return t;
  }
  
  
  PPtr<KString> KernelRuntime::getMessageOpCodeForHash(const k_longint_t hash)
  const
  {
    PPtr<KString> s = RuntimeBase::getMessageOpCodeForHash(hash);
    if(s.isNull()) {
      for(int i = _virtualRuntimes->getSize() - 1; i >= 0; i--) {
        s = _virtualRuntimes->at(i)->getMessageOpCodeForHash(hash);
        if(!s.isNull()) {
          return s;
        }
      }
    }
    return s;
  }
  
  
// Inherited from RuntimeBase::Runtime //
  
  const k_guid_t& KernelRuntime::getConsoleGuid() const {
    return _kernelAgent->getConsoleGuid();
  }
  
  
  bool KernelRuntime::isHead() const {
    return _kernelAgent->isHead();
  }
  
    
  void KernelRuntime::send(const k_guid_t& sender, const k_guid_t& receiver,
       const k_longint_t opcode, PPtr<KValue> content, const k_integer_t tid)
  {
    if(_closing) {
      return;
    }
    
    Ptr<Message> msg = _messagePool.get();
    msg->set(tid, opcode, sender, content);
        
    deliver(receiver, msg);
  }
  
  
  void KernelRuntime::send(const k_guid_t& sender, PPtr<Group> receivers,
       const k_longint_t opcode, PPtr<KValue> content, const k_integer_t tid)
  {
    if(_closing) {
      return;
    }
    
    Ptr<Message> msg = _messagePool.get();
    msg->set(tid, opcode, sender, content);
    
    for(int i = receivers->getCount() - 1; i >= 0; i--) {
      deliver(receivers->get(i), msg);
    }
  }
  
  
  void KernelRuntime::sendToAll(const k_guid_t& sender, const k_longint_t opcode,
      PPtr<KValue> content, const k_integer_t tid)
  {
    if(_closing) {
      return;
    }
    
    Ptr<Message> msg = _messagePool.get();
    msg->set(tid, opcode, sender, content);
    
    deliverToAll(msg);
    
    _kernelAgent->proxySendToAll(msg);
  }
  
  
  void KernelRuntime::sendToLocals(const k_guid_t& sender, const k_longint_t opcode,
      PPtr<KValue> content, const k_integer_t tid)
  {
    if(_closing) {
      return;
    }
    
    Ptr<Message> msg = _messagePool.get();
    msg->set(tid, opcode, sender, content);
    
    deliverLocally(msg);
  }
  
  
// Inherited from RuntimeBase::SerializingStreamer //
  
  void KernelRuntime::serialize(PPtr<ObjectSerializer> serializer) const {
    serializer->object("KernelRuntime")
      ->attribute("guid", KGlobalUid::toString(getGuid()))
      ->member("virtualRuntimes")
      ->object< ManagedArray<VirtualRuntime> >(_virtualRuntimes)
      ->endObject();
  }


} // namespace are
} // namespace knorba
