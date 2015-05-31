//
//  AgentDatabase.cpp
//  KnoRBA-ARE-XCodeWrapper
//
//  Created by Hamed KHANDAN on 9/8/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/Logger.h>
#include <kfoundation/Mutex.h>
#include <kfoundation/ManagedArray.h>

// KnoRBA
#include <knorba/type/KGlobalUid.h>
#include <knorba/AgentLoader.h>
#include <knorba/Group.h>

// Internal
#include "AgentInstance.h"
#include "RuntimeBase.h"

// Self
#include "AgentDatabase.h"

namespace knorba {
namespace are {
  
  using namespace std;
  
// --- (DE)CONSTRUCTORS --- //

  AgentDatabase::AgentDatabase(RuntimeBase& rt)
  : _rt(rt)
  {
    _counter = 1;
    _loaders = new ManagedArray<AgentLoader>();
    _instances = new ManagedArray<AgentInstance>();
  }
  
  
// --- METHODS --- //

  void AgentDatabase::registerLoader(PPtr<AgentLoader> loader) {
    _loaders->push(loader);
  }
  
  
  PPtr<AgentInstance> AgentDatabase::instantiate(const string& className) {
    return instantiate(className, className + Int::toString(_counter));
  }
  
  
  PPtr<AgentInstance> AgentDatabase::instantiate(const string& className,
    const string& alias)
  {
    PPtr<AgentLoader> loader;
    
    for(int i = _loaders->getSize() - 1; i >= 0; i--) {
      if(_loaders->at(i)->getClassName() == className) {
        loader = _loaders->at(i);
        break;
      }
    }
    
    if(loader.isNull()) {
      LOG_ERR << "Agent could not be instantiated. No agent loader found for \""
          << className << "\"" << EL;
      return NULL;
    }
    
    return instantiate(loader, alias);
  }
  
  
  PPtr<AgentInstance> AgentDatabase::instantiate(PPtr<AgentLoader> loader,
      const string& alias)
  {
    PPtr<Path> resourcePath = loader->getPathToResources();

    k_guid_t instanceId = _rt.getGuid();
    KGlobalUid::randomizeKey(instanceId);
    
    Ptr<Path> dataPath;
    if(!_rt.getDataPath().isNull()) {
      dataPath = _rt.getDataPath()->addSegement(alias);
      if(!dataPath->exists()) {
        dataPath->makeDir();
      }
    }
    
    _mutex.lock();
    instanceId.lid = _counter;
    _counter++;
    
    Ptr<AgentInstance> newInstance;
    
    newInstance = new AgentInstance(instanceId, instanceId.key, loader, alias,
        resourcePath, dataPath);
    
    _instances->push(newInstance);
    _mutex.unlock();
    
    newInstance->setObject(loader->instantiate(_rt, instanceId));
    
    return newInstance;
  }
  
  
  int AgentDatabase::getNInstances() const {
    return _instances->getSize();
  }
  
  
  PPtr<AgentInstance> AgentDatabase::getInstanceByIndex(const int index) const {
    return _instances->at(index);
  }
  
  
  PPtr<AgentInstance> AgentDatabase::getInstanceByGuid(const k_guid_t &uid)
  const
  {
    if(uid.lid > _instances->getSize()) {
      return PPtr<AgentInstance>();
    }
    
    PPtr<AgentInstance> instance = _instances->at(uid.lid - 1);
    
    if(instance->_guid.key != uid.key) {
      return PPtr<AgentInstance>();
    }
    
    return instance;
  }
  
  
  PPtr<AgentInstance> AgentDatabase::getInstanceByAlias(const string& alias)
  const
  {
    for(int i = _instances->getSize() - 1; i >= 0; i--) {
      PPtr<AgentInstance> instance = _instances->at(i);
      if(instance->getAlias() == alias) {
        return instance;
      }
    }
    
    return PPtr<AgentInstance>();
  }
  
  
  PPtr<AgentInstance> AgentDatabase::getInstanceByPointer(const Agent* ptr)
  const {
    for(int i = _instances->getSize() - 1; i >= 0; i--) {
      PPtr<AgentInstance> instance = _instances->at(i);
      if(instance->getObject() == ptr) {
        return instance;
      }
    }
    
    return NULL;
  }
  
  
  int AgentDatabase::getIndexByGuid(const k_guid_t &uid) const {
    if(!KGlobalUid::areOnTheSameNode(uid, _rt.getGuid())) {
      return -1;
    }
    
    if(uid.lid > _instances->getSize()) {
      return -1;
    }
    
    if(uid.key != _instances->at(uid.lid - 1)->_guid.key) {
      return -1;
    }
    
    return uid.lid - 1;
  }
  
  
  PPtr<AgentLoader> AgentDatabase::getLoaderForClass(const string& className)
  const
  {
    for(int i = _loaders->getSize() - 1; i >= 0; i--) {
      if(_loaders->at(i)->getClassName() == className) {
        return _loaders->at(i);
      }
    }
    
    return NULL;
  }
  
  
  void AgentDatabase::serialize(PPtr<ObjectSerializer> serializer) const {
    serializer->object("AgentDatabase")
        ->member("loaders")->object< ManagedArray<AgentLoader> >(_loaders)
        ->member("instances")->object< ManagedArray<AgentInstance> >(_instances)
        ->endObject();
  }
  
} // are
} // knorba