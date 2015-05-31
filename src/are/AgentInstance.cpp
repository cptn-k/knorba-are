//
//  AgentInstance.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 9/18/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

// Unix
#include <unistd.h>

// KFoundation
#include <kfoundation/Ptr.h>

// KnoRBA
#include <knorba/type/KGlobalUid.h>
#include <knorba/AgentLoader.h>
#include <knorba/Agent.h>

// Internal
#include "AgentInstance.h"

namespace knorba {
namespace are {
  
  using namespace knorba::type;

// --- (DE)CONSTRUCTORS --- //
  
  AgentInstance::AgentInstance(const k_guid_t& uid, const k_integer_t key,
      PPtr<AgentLoader> loader, const string& alias,
      PPtr<Path> pathToResources, PPtr<Path> pathToData)
  : _guid(uid),
    _key(key),
    _alias(alias),
    _object(NULL)
  {
    _loader = loader;
    _pathToResources = pathToResources;
    _pathToData = pathToData;
  }
  
  
  AgentInstance::~AgentInstance() {
    if(_object != NULL) {
      delete _object;
    }
  }
  
  
// --- METHODS --- //
  
  void AgentInstance::setObject(Agent* const obj) {
    if(_object != NULL) {
      throw KFException("Agent is already instantiated.");
    }
    
    _object = obj;
  }
  
  
  const string& AgentInstance::getAlias() const {
    return _alias;
  }
  
  
  PPtr<Path> AgentInstance::getPathToResources() const {
    return _pathToResources;
  }
  
  
  PPtr<Path> AgentInstance::getPathToData() const {
    return _pathToData;
  }
  
  
  PPtr<AgentLoader> AgentInstance::getLoader() const {
    return _loader;
  }
  
  
  bool AgentInstance::isAlive() const {
    if(IS_NULL(_object)) {
      return false;
    }
    
    return _object->isAlive();
  }

  
  void AgentInstance::deleteAgent() {
    if(IS_NULL(_object)) {
      return;
    }
    
    LOG << "Deleting: " << *this << EL;
    
    delete _object;
    _object = NULL;
  }
  
  
  void AgentInstance::serialize(PPtr<ObjectSerializer> serializer) const {
    serializer->object("AgentInstance")
              ->attribute("class", _loader->getClassName())
              ->attribute("guid", KGlobalUid::toString(_guid))
              ->attribute("alias", _alias);
    
    if(!_pathToResources.isNull()) {
      serializer->attribute("pathToResources", _pathToResources->getString());
    }
    
    if(!_pathToData.isNull()) {
      serializer->attribute("pathToData", _pathToData->getString());
    }
    
    serializer->endObject();
  }
  
} // namespace are
} // namespace knorba
