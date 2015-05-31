//
//  AgentClass.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/23/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// Posix
#include <dlfcn.h>
#include <unistd.h>

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/Path.h>
#include <kfoundation/System.h>
#include <kfoundation/MasterMemoryManager.h>
#include <kfoundation/Logger.h>

// Internal
#include "AreException.h"

// Self
#include "DynamicAgentLoader.h"

namespace knorba {
namespace are {
  
// --- STATIC FIELDS --- //

#ifdef KF_LINUX
  const string DynamicAgentLoader::EXTENSION = ".so";
#elif defined(KF_MAC)
  const string DynamicAgentLoader::EXTENSION = ".dylib";
#endif
  
  
// --- (DE)CONSTRUCTORS --- //
  
  DynamicAgentLoader::DynamicAgentLoader(PPtr<Path> kap, const string& className)
  : AgentLoader(className, kap->addSegement("resources"))
  {
    _binary = kap->addSegement("bin/" + className + EXTENSION);
    
    _handle = dlopen(_binary->getString().c_str(), RTLD_LAZY);
    if(_handle == NULL) {
      throw AreException("Unable to load agent binary " + _binary->getString());
    }
    
    System::takeover(_handle);
    _managerIndex = System::getMasterMemoryManager().getNManagers() - 1;
    
    _initFn = (init_t)dlsym(_handle, "init");
    _instantiateFn = (instantiate_t)dlsym(_handle, "instantiate");
    if(_instantiateFn == NULL) {
      throw KFException("Instantiation function not found in binary for \""
          + getClassName() + "\"");
    }
    
    LOG << "(O) Agent image loaded: " << _binary->getFileNameWithExtension() << EL;
  }
  
  
  DynamicAgentLoader::~DynamicAgentLoader() {
    if(dlclose(_handle) != 0) {
      LOG_ERR << "Could not close dynamic library for agent " << getClassName()
          << ". Reason: " << System::getLastSystemError() << EL;
    }
    
    System::getMasterMemoryManager().unregisterManager(_managerIndex);
    
    LOG << "(X) Agent image unloaded: " << _binary->getFileNameWithExtension() << EL;
  }
  
  
// --- METHODS --- //
  
  PPtr<Path> DynamicAgentLoader::getBinaryPath() const {
    return _binary;
  }
  
  
  void DynamicAgentLoader::init(Runtime &rt) {
    if(NOT_NULL(_initFn)) {
      _initFn(rt);
    } else {
      LOG_ERR << "No init() in " << *_binary << EL;
    }
  }
  
  
  Agent* DynamicAgentLoader::instantiate(Runtime& rt,
      const k_guid_t& guid)
  {
    return _instantiateFn(rt, guid);
  }
    
  
} // namespace are
} // namespace knorba
