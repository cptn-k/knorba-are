//
//  AgentClass.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/23/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__AgentClass__
#define __AgentRuntimeEnvironment__AgentClass__

// KnoRBA
#include <knorba/type/definitions.h>

// Super
#include <knorba/AgentLoader.h>

namespace kfoundation {
  class Path;
}

namespace knorba {
  class Agent;
  class Runtime;
}

namespace knorba {
namespace are {
  
  using namespace kfoundation;
  using namespace knorba::type;
  
  class DynamicAgentLoader : public AgentLoader {
  
  // --- NESTED DATATYPES --- //
    
    private: typedef void (*init_t)(Runtime& rt);
    private: typedef Agent* (*instantiate_t)(Runtime& rt, const k_guid_t& guid);
    
    
  // --- STATIC FIELDS --- //
    
    private: static const string EXTENSION;
    
    
  // --- FIELDS --- //
    
    private: Ptr<Path> _binary;
    private: void* _handle;
    private: init_t _initFn;
    private: instantiate_t _instantiateFn;
    private: kf_octet_t _managerIndex;

    
  // --- (DE)CONSTRUCTOR --- //
    
    public: DynamicAgentLoader(PPtr<Path> kap, const string& className);
    public: ~DynamicAgentLoader();
    
    
  // --- METHODS --- //
    
    public: PPtr<Path> getBinaryPath() const;
    
    // Inherited from AgentLoader
    public: void init(Runtime& rt);
    public: Agent* instantiate(Runtime& rt, const k_guid_t& guid);
    
  };
  
} // namespace are
} // namespace knorba

#endif /* defined(__AgentRuntimeEnvironment__AgentClass__) */
