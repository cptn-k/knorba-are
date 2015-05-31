//
//  AgentMetadata.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 11/7/14.
//  Copyright (c) 2014 Kay Khandan. All rights reserved.
//

// Posix
#include <dlfcn.h>
#include <unistd.h>

// KFoundation
#include <kfoundation/Ptr.h>

// Internal
#include "AgentMetadata.h"
#include "Bundle.h"
#include "RuntimeImpl.h"
#include "AreException.h"


namespace knorba {
namespace are {
   
//\/ AgentMetadata::Loader /\//////////////////////////////////////////////////
  
  AgentMetadata::Loader::Loader(PPtr<Path> binary, const string& name,
      const string& bundleName, PPtr<Path> resources)
  : AgentLoader(name, bundleName, resources)
  {
    _handle = dlopen(binary->getString().c_str(), RTLD_LAZY);
    if(_handle == NULL) {
      throw AreException("Unable to load binary for agent at " + binary->getString());
    }
    
    System::takeover(_handle);
    _managerIndex = System::getMasterMemoryManager().getNManagers() - 1;
    
    _initFn = (init_t)dlsym(_handle, "init");
    _instantiateFn = (instantiate_t)dlsym(_handle, "instantiate");
    if(_instantiateFn == NULL) {
      throw KFException("Instantiation function not found for agent " + name);
    }
  }
  
  
  AgentMetadata::Loader::~Loader() {
    delete System::getMasterMemoryManager().getManagerAtIndex(_managerIndex);
    
    if(dlclose(_handle) != 0) {
      LOG_ERR << "Could not close dynamic library for agent " << *getClassName()
      << ". Reason: " << System::getLastSystemError() << EL;
    }
  }
  
  
  void AgentMetadata::Loader::init(Runtime& rt) {
    if(_initFn != NULL) {
      _initFn(rt);
    }
  }
  
  
  Agent* AgentMetadata::Loader::instantiate(Runtime& rt, const k_guid_t &guid)
  {
    return _instantiateFn(rt, guid);
  }
  

//\/ AgentMetadata /\//////////////////////////////////////////////////////////
  
// --- STATIC FIELDS --- //

#ifdef KF_LINUX
  const string AgentMetadata::EXTENSION = ".so";
#elif defined(KF_MAC)
  const string AgentMetadata::EXTENSION = ".dylib";
#endif

  const string AgentMetadata::AGENT  = "Agent";
  const string AgentMetadata::NAME   = "name";
  const string AgentMetadata::SOURCE = "Source";
  const string AgentMetadata::PATH   = "path";
    
    
// --- (DE)CONSTRUCTORS --- //

  AgentMetadata::AgentMetadata() {
    _sources = new Array<string>();
  }
  
  
  AgentMetadata::~AgentMetadata() {
    // Nothing;
  }
  
  
// --- METHODS --- //
  
  void AgentMetadata::setName(const string& name) {
    _name = name;
  }
  
  
  const string& AgentMetadata::getName() const {
    return _name;
  }
  
  
  void AgentMetadata::addSource(const string& path) {
    _sources->push(path);
  }
  
  
  void AgentMetadata::clearSources() {
    _sources->clear();
  }
  
  
  bool AgentMetadata::hasSource() const {
    return !_sources->isEmpty();
  }
  
  
  Array<string>::PPtr_t AgentMetadata::getSources() const {
    return _sources;
  }
  
  
  void AgentMetadata::setBinaryPath(PPtr<Path> path) {
    _binary = path;
  }
  
  
  PPtr<Path> AgentMetadata::getBinaryPath() const {
    return _binary;
  }
  
  
  PPtr<AgentLoader> AgentMetadata::getLoader() const {
    return _loader;
  }
  
  
  void AgentMetadata::compile(string command, PPtr<Path> target) {
    // Nothing;
  }
  
  
  void AgentMetadata::readFromObjectStream(PPtr<ObjectToken> headToken,
      const string& bundleName, PPtr<Path> bundleRoot)
  {
    headToken->validateClass(AGENT);
    
    Ptr<Token> token = headToken->next();
    token->validateType(Token::ATTRIBUTE);
    setName(token.AS(AttributeToken)->validateName(NAME)->getValue());
    
    token = token->next();
    while(token->is(Token::OBJECT)) {
      PPtr<ObjectToken> object = token.AS(ObjectToken);
      object->validateClass(SOURCE);
      
      token = headToken->next();
      token->validateType(Token::ATTRIBUTE);
      addSource(token.AS(AttributeToken)->validateName(PATH)->getValue());
      
      token = headToken->next();
      token->validateType(Token::END_OBJECT);
      
      token = headToken->next();
    }
    
    token->validateType(Token::END_OBJECT);
    
    _binary = bundleRoot->addSegement("bin/" + _name + EXTENSION);
    _resources = bundleRoot->addSegement("resources");
    
    _loader = new Loader(_binary, _name, bundleName, _resources);
  }
  
  
  void AgentMetadata::serialize(PPtr<ObjectSerializer> serializer) const {
    serializer->object(AGENT);
    serializer->attribute(NAME, _name);
    for(int i = 0; i < _sources->getSize(); i++) {
      serializer->member("source")->object(SOURCE)
                ->attribute(PATH, _sources->get(i))
                ->endObject();
    }
    serializer->endObject();
  }
      
} // are
} // knorba