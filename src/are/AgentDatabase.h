//
//  AgentDatabase.h
//  KnoRBA-ARE-XCodeWrapper
//
//  Created by Hamed KHANDAN on 9/8/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef KNORBA_ARE_AGENTDATABASE
#define KNORBA_ARE_AGENTDATABASE

// KFoundation
#include <kfoundation/ManagedObject.h>
#include <kfoundation/SerializingStreamer.h>

namespace kfoundation {
  class Mutex;
}

namespace knorba {
  class AgentLoader;
  class Runtime;
}

namespace knorba {
namespace are {
  
  using namespace kfoundation;
  using namespace knorba;
  
  class AgentInstance;
  class RuntimeBase;
  
  class AgentDatabase : public ManagedObject, public SerializingStreamer {
    
  // --- FIELDS --- //
    
    private: Mutex _mutex;
    private: RuntimeBase& _rt;
    private: k_integer_t _counter;
    private: Ptr< ManagedArray<AgentLoader> > _loaders;
    private: Ptr< ManagedArray<AgentInstance> > _instances;
    
    
  // --- (DE)CONSTRUCTORS --- //
    
    public: AgentDatabase(RuntimeBase& rt);
    
    
  // --- METHODS --- //
    
    public: void registerLoader(PPtr<AgentLoader> loader);
    public: PPtr<AgentInstance> instantiate(const string& className);
    
    public: PPtr<AgentInstance> instantiate(const string& className,
        const string& alias);
    
    public: PPtr<AgentInstance> instantiate(PPtr<AgentLoader> loader,
        const string& alias);
    
    public: int getNInstances() const;
    public: PPtr<AgentInstance> getInstanceByIndex(const int index) const;
    public: PPtr<AgentInstance> getInstanceByGuid(const k_guid_t& uid) const;
    public: PPtr<AgentInstance> getInstanceByAlias(const string& alias) const;
    public: PPtr<AgentInstance> getInstanceByPointer(const Agent* ptr) const;
    public: int getIndexByGuid(const k_guid_t& uid) const;
    public: PPtr<AgentLoader> getLoaderForClass(const string& className) const;
    
    // From SerializingStreamer
    void serialize(PPtr<ObjectSerializer> serializer) const;
  };
  
} // namespace are
} // namespace knorba

#endif /* defined(KNORBA_ARE_AGENTDATABASE) */
