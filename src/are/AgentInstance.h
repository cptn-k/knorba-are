//
//  AgentInstance.h
//  ARE
//
//  Created by Hamed KHANDAN on 9/18/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef __ARE__AgentInstance__
#define __ARE__AgentInstance__

// KFoundation
#include <kfoundation/ManagedObject.h>
#include <kfoundation/SerializingStreamer.h>

namespace knorba {
  class AgentLoader;
}

namespace knorba {
namespace are {
  
  using namespace kfoundation;
  using namespace knorba::type;
  using namespace knorba;
  
  class AgentInstance : public ManagedObject, public SerializingStreamer {
    
  // --- FIELDS --- //
    
    public : const k_guid_t       _guid;
    public : const k_integer_t _key;
    private: Agent*       _object;
    private: const string _alias;
    private: Ptr<Path>    _pathToResources;
    private: Ptr<Path>    _pathToData;
    private: Ptr<AgentLoader> _loader;
    
    
  // --- (DE)CONSTRUCTORS --- //
    
    public: AgentInstance(const k_guid_t& guid, const k_integer_t key,
        PPtr<AgentLoader> loader, const string& alias,
        PPtr<Path> pathToResources, PPtr<Path> pathToData);
    
    public: ~AgentInstance();
    
    
  // --- METHODS --- //
    
    public: void setObject(Agent* const obj);
    public: inline Agent* getObject() const;
    public: const string& getAlias() const;
    public: PPtr<Path> getPathToResources() const;
    public: PPtr<Path> getPathToData()const;
    public: PPtr<AgentLoader> getLoader() const;
    public: bool isAlive() const;
    public: void deleteAgent();
    
    // From SerializingStreamer
    public: void serialize(PPtr<ObjectSerializer> serializer) const;
  };
  
  inline Agent* AgentInstance::getObject() const {
    return _object;
  }
  
} // namespace are
} // namespace knorba

#endif /* defined(__ARE__AgentInstance__) */
