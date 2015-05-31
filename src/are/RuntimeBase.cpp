//
//  RuntimeBase.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/26/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/FileInputStream.h>
#include <kfoundation/Path.h>
#include <kfoundation/XmlObjectStreamReader.h>
#include <kfoundation/SerializingStreamer.h>

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/Message.h>
#include <knorba/type/KGlobalUid.h>

// Internal
#include "OpcodeTable.h"
#include "Bundle.h"
#include "AreException.h"
#include "ManifestoElement.h"
#include "LoadElement.h"
#include "AgentInstance.h"
#include "AgentDatabase.h"
#include "RunElement.h"
#include "MessageElement.h"
#include "DynamicAgentLoader.h"
#include "PeerElement.h"
#include "MessageElement.h"

// Self
#include "RuntimeBase.h"

namespace knorba {
namespace are {


  /**
   * Primary constructor
   */
  RuntimeBase::RuntimeBase(PPtr<Path> knorbaHome, const k_guid_t& guid)
  : _agentQuitCond(false)
  {
    _nodeCount = 1;
    _closing = false;
    _guid = guid;
    _appName = "";
    _knorbaHome = knorbaHome;
    _opcodeTable = new OpcodeTable();
    _db = new AgentDatabase(*this);
    
    _opcodeTable->registerRecord(Agent::OP_CONNECT, KType::STRING);
  }
  
  
  RuntimeBase::~RuntimeBase() {
    _opcodeTable = NULL;
  }


// --- METHODS --- //

  void RuntimeBase::logBlockingAgents(Logger::Stream& stream) const {
    bool isFirst = true;
    for(int i = _db->getNInstances() - 1; i >= 0; i--) {
      PPtr<AgentInstance> instance = _db->getInstanceByIndex(i);
      if(instance->isAlive()) {
        if(isFirst) {
          stream << " ";
        } else {
          stream << ", ";
        }
        stream << instance->getObject()->getAlias();
        isFirst = false;
      }
    }
    stream << EL;
  }
  
  
  void RuntimeBase::attemptStart(PPtr<AgentInstance> instance) {
    instance->getObject()->run();
    
    for(int i = 0; i < 10; i++) {
      System::sleep(20);
      if(instance->isAlive()) {
        return;
      }
    }
    
    LOG_ERR << "Agent " << instance->_guid << " a.k.a. \""
        << instance->getAlias() << "\" failed to start." << EL;
  }
  
  
  void RuntimeBase::setNodeCount(k_integer_t n) {
    _nodeCount = n;
    LOG << "NNodes Set: " << n << EL;
  }
  
  
  PPtr<Path> RuntimeBase::getHome() const {
    return _knorbaHome;
  }

  
  PPtr<Path> RuntimeBase::getDataPath() const {
    return _dataPath;
  }
  
  
  PPtr<Path> RuntimeBase::getKapPath() const {
    return _kapPath;
  }
  
  
  const string& RuntimeBase::getAppName() const {
    return _appName;
  }
  
  
  void RuntimeBase::runBundle(PPtr<Path> path) throw(KFException) {
    Ptr<Bundle> bundle = new Bundle(path, _knorbaHome);
    bundle->pack();
    runBundle(bundle);
  }
  
  
  void RuntimeBase::runBundle(PPtr<Bundle> bundle) throw(KFException) {
    _kapPath = bundle->getKapPath();
    _appName = bundle->getAppName();
    _dataPath = _knorbaHome->addSegement("data")->addSegement(_appName);
    if(!_dataPath->exists()) {
      _dataPath->makeDir();
    }
    
    PPtr<Path> manifestoPath = bundle->getManifestoPath();
    if(!manifestoPath->exists()) {
      throw AreException("Could not find manifesto file for app '"
          + bundle->getAppName() + "'");
    }

    Ptr<ManifestoElement> manifesto = new ManifestoElement(*this);

    try {
      Ptr<FileInputStream> fis = new FileInputStream(manifestoPath);
      manifesto->readFromXmlStream(fis.AS(InputStream));
    } catch(KFException e) {
      LOG_ERR << e.getMessage() << EL;
      throw AreException("Error occured while reading "
          + manifestoPath->getString());
    }
    
    LOG << "Running manifesto: " << *manifesto << EL;
    
    PPtr< ManagedArray<RunElement> > runElements = manifesto->getRunElements();
    for(int i = 0; i < runElements->getSize(); i++) {
      PPtr<RunElement> e = runElements->at(i);
      if(e->hasAlias()) {
        attemptStart(_db->instantiate(e->getClassName(), e->getAlias()));
      } else {
        attemptStart(_db->instantiate(e->getClassName()));
      }
    }
    
    PPtr< ManagedArray<PeerElement> > peerElements
       = manifesto->getPeerElements();
    
    for(int i = 0; i < peerElements->getSize(); i++) {
      PPtr<PeerElement> e = peerElements->at(i);
      
      PPtr<AgentInstance> source = _db->getInstanceByAlias(e->getSource());
      if(source.isNull()) {
        LOG_ERR << "Unable to resolve alias name for source agent in " << *e
            << EL;
        continue;
      }
      
      PPtr<AgentInstance> target = _db->getInstanceByAlias(e->getTarget());
      if(target.isNull()) {
        LOG_ERR << "Unable to resolve alias name for target agent in " << *e
            << EL;
        continue;
      }
      
      target->getObject()->handlePeerConnectionRequest(
          new KString(e->getRole()), source->_guid);
    }
    
    PPtr< ManagedArray<MessageElement> > messageElements
        = manifesto->getMessageElements();
    
    for(int i = 0; i < messageElements->getSize(); i++) {
      PPtr<MessageElement> e = messageElements->at(i);
      
      PPtr<AgentInstance> target = _db->getInstanceByAlias(e->getTarget());
      if(target.isNull()) {
        LOG_ERR << "Unable to resolve alias name for target in " << *e
            << EL;
        continue;
      }
      
      sendFromAre(target->_guid, e->getOpcode(), e->getPayload());
    }
  }
  
  
  void RuntimeBase::unload() {
    _opcodeTable = NULL;
    _db = NULL;
  }


  void RuntimeBase::loadClass(const string& className) {
    Ptr<AgentLoader> loader = createLoaderFor(_kapPath, className);
    loader->init(*this);
    _db->registerLoader(loader);
  }

  
  void RuntimeBase::registerLoader(PPtr<AgentLoader> loader) {
    _db->registerLoader(loader);
  }
  
  
  PPtr<AgentLoader> RuntimeBase::getLoaderForClass(const string& className)
  const
  {
    return _db->getLoaderForClass(className);
  }
  
  
  PPtr<AgentInstance> RuntimeBase::getInstanceByGuid(const k_guid_t& guid) const {
    return _db->getInstanceByGuid(guid);
  }
  
  
  PPtr<AgentInstance> RuntimeBase::instantiate(PPtr<AgentLoader> loader,
      const string &alias)
  {
    PPtr<AgentInstance> instance = _db->instantiate(loader, alias);
    instance->getObject()->run();
    return instance;
  }

    
  bool RuntimeBase::deliverLocally(const k_guid_t& receiver, PPtr<Message> msg) {
    if(!KGlobalUid::areOnTheSameNode(receiver, _guid)) {
      return false;
    }
    
    PPtr<AgentInstance> target = getInstanceByGuid(receiver);
    if(target.isNull()) {
      LOG_ERR << "Message could not be delivered. Target agent does not exist. "
          << msg->headerToString(*this) << EL;
    } else if(!target->isAlive()) {
      LOG << *target << EL;
      LOG_WRN << "Message could not be delivered. Target agent is dead. "
          << msg->headerToString(*this) << EL;
    } else {
      msg.retain();
      target->getObject()->processMessage(msg);
    }
    
    return true;
  }
  
  
  void RuntimeBase::deliverLocally(PPtr<Message> msg) {
    for(int i = _db->getNInstances() - 1; i >= 0; i--) {
      PPtr<AgentInstance> instance = _db->getInstanceByIndex(i);
      if(instance->isAlive()) {
        Agent* agent = instance->getObject();
        if(agent->getGuid() == msg->getSender()) {
          continue;
        }
        msg.retain();
        agent->processMessage(msg);
      }
    }
  }
  
  
  void RuntimeBase::waitWhileRunning() {
    while(isActive() && !_closing) {
      _agentQuitCond.block();
      cleanup();
      logBlockingAgents(LOG << "Quit signal received. Active agent(s):");
    }
  }
  
  
  bool RuntimeBase::isAlive() const {
    for(int i = _db->getNInstances() - 1; i >= 0; i--) {
      if(_db->getInstanceByIndex(i)->isAlive()) {
        return true;
      }
    }
    return false;
  }
  
  
  bool RuntimeBase::isActive() const {
    for(int i = _db->getNInstances() - 1; i >= 0; i--) {
      PPtr<AgentInstance> instance = _db->getInstanceByIndex(i);
      if(instance->isAlive()) {
        if(!instance->getObject()->isPassive()) {
          return true;
        }
      }
    }
    return false;
  }
  
  
  void RuntimeBase::shutdown() {
    _closing = true;
    
    for(int i = _db->getNInstances() - 1; i >= 0; i--) {
      PPtr<AgentInstance> instance = _db->getInstanceByIndex(i);
      if(instance->isAlive()) {
        instance->getObject()->quit();
      }
    }
    
    int counter = 0;
    while(isAlive()) {
      System::sleep(1000);
      counter++;
      if(counter == 5) {
        logBlockingAgents(LOG_ERR << "Shutdown blocked by these agent(s):");
      }
    }
  }
  
  
  void RuntimeBase::release() {
    _closing = true;
    _agentQuitCond.release();
  }
  
  
  void RuntimeBase::signalQuit() {
    _agentQuitCond.release();
  }
  
  
  void RuntimeBase::cleanup() {
    for(int i = _db->getNInstances() - 1; i >= 0; i--) {
      PPtr<AgentInstance> instance = _db->getInstanceByIndex(i);
      if(!instance->isAlive()) {
        if(NOT_NULL(instance->getObject())) {
          k_guid_t guid = instance->getObject()->getGuid();
          instance->deleteAgent();
          if(!_closing) {
            handleAgentDisconnect(guid);
          }
        }
      }
    }
  }
  
  
  void RuntimeBase::handleRuntimeDisconnect(const k_guid_t& rt) {
    if(KGlobalUid::areOnTheSameApp(rt, _guid)) {
      if(_nodeCount == 1) {
        LOG_ERR << "Node count decremented too many times. " << _guid << EL;
      } else {
        _nodeCount--;
        LOG << "NNodes Dec: " << _nodeCount << EL;
      }
    }
    
    for(int i = _db->getNInstances() - 1; i >= 0; i--) {
      PPtr<AgentInstance> instance = _db->getInstanceByIndex(i);
      if(instance->isAlive()) {
        instance->getObject()->removeAllPeersWithMatchingAppId(rt);
      }
    }
  }
  
  
  void RuntimeBase::handleAgentDisconnect(const k_guid_t& agent) {
    for(int i = _db->getNInstances() - 1; i >= 0; i--) {
      PPtr<AgentInstance> instance = _db->getInstanceByIndex(i);
      if(instance->isAlive()) {
        Agent* a = instance->getObject();
        PPtr<KString> role = a->getRole(agent);
        if(!role.isNull()) {
          a->removePeer(role, agent);
          a->handlePeerDisconnected(role, agent);
        }
      }
    }
  }
  
  
// Inherited from Runtime //

  const k_guid_t& RuntimeBase::getGuid() const {
    return _guid;
  }
  
  
  k_integer_t RuntimeBase::getNodeCount() const {
    return _nodeCount;
  }

  
  PPtr<Path> RuntimeBase::getResourcePathForAgent(const Agent* itself) const {
    PPtr<AgentInstance> instance = _db->getInstanceByGuid(itself->getGuid());
    if(instance.isNull()) {
      throw KFException("getResourcePathForAgent() is given pointer to an "
          "invalid agent as parameter.");
    }
    return instance->getPathToResources();
  }
  
  
  PPtr<Path> RuntimeBase::getDataPathForAgent(const Agent* itself) const {
    PPtr<AgentInstance> instance = _db->getInstanceByGuid(itself->getGuid());
    if(instance.isNull()) {
      throw KFException("getDataPathForAgent() is given pointer to an "
                        "invalid agent as parameter.");
    }
    return instance->getPathToData();
  }
  
  
  const string& RuntimeBase::getClassNameForAgent(const Agent* itself) const {
    PPtr<AgentInstance> instance = _db->getInstanceByGuid(itself->getGuid());
    if(instance.isNull()) {
      throw KFException("getClassNameForAgent() is given pointer to an "
          "invalid agent as parameter.");
    }
    return instance->getLoader()->getClassName();
  }
  
  
  const string& RuntimeBase::getAliasForAgent(const Agent* itself) const {
    PPtr<AgentInstance> instance = _db->getInstanceByGuid(itself->getGuid());
    if(instance.isNull()) {
      throw KFException("getClassNameForAgent() is given pointer to an "
          "invalid agent as parameter.");
    }
    return instance->getAlias();
  }


  const k_guid_t& RuntimeBase::getAgentGuidByAlias(const string& alias) const {
    PPtr<AgentInstance> instance = _db->getInstanceByAlias(alias);
    if(instance.isNull()) {
      KGlobalUid::zero();
    }
    return instance->_guid;
  }


  void RuntimeBase::registerType(PPtr<KType> type) {
    throw KFException("registerType() is not implemented.");
  }


  PPtr<KType> RuntimeBase::getTypeByHash(const k_longint_t hash) const {
    return _opcodeTable->getTypeByNameHash(hash);
  }


  void RuntimeBase::registerMessageFormat(PPtr<KString> opcode,
      PPtr<KType> payloadType)
  {
    _opcodeTable->registerRecord(opcode, payloadType);
  }


  PPtr<KType> RuntimeBase::getMessageFormatByHash(const k_longint_t hash)
  const
  {
    return _opcodeTable->getTypeByOpcodeHash(hash);
  }


  PPtr<KString> RuntimeBase::getMessageOpCodeForHash(const k_longint_t hash)
  const
  {
    return _opcodeTable->getOpcode(hash);
  }
  
  
// Inherited from SerializingStreamer //
  
  void RuntimeBase::serialize(PPtr<ObjectSerializer> serialzier) const {
    serialzier->object("Runtime")
      ->attribute("guid", KGlobalUid::toString(_guid))
      ->attribute("appName", _appName)
      ->endObject();
  }
  
  
} // namespace are
} // namespace knorba
