//
//  RuntimeBase.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/26/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__RuntimeBase__
#define __AgentRuntimeEnvironment__RuntimeBase__

// KFoundation
#include <kfoundation/Condition.h>

// Super
#include <knorba/Runtime.h>

namespace knorba {
  class AgentLoader;
  class Message;
}

namespace knorba {
namespace are {
    
  class OpcodeTable;
  class KernelAgent;
  class LoadElement;
  class AgentDatabase;
  class AgentInstance;
  class Bundle;
  
  class RuntimeBase: public Runtime, public ManagedObject,
      public SerializingStreamer
  {
      
  // --- FIELDS --- //
      
    private: k_guid_t _guid;
    private: k_integer_t _nodeCount;
    private: string _appName;
    private: Ptr<Path> _kapPath;
    private: bool _closing;
    private: Condition _agentQuitCond;
    private: Ptr<Path> _knorbaHome;
    private: Ptr<Path> _dataPath;
    private: Ptr<OpcodeTable> _opcodeTable;
    private: Ptr<AgentDatabase> _db;
    
      
  // --- (DE)CONSTRUCTORS --- //
      
    public: RuntimeBase(PPtr<Path> knorbaHome, const k_guid_t& guid);
    public: ~RuntimeBase();
    
    
  // --- METHODS --- //
    
    private: void logBlockingAgents(Logger::Stream& stream) const;
    private: void attemptStart(PPtr<AgentInstance> instance);
    
    public: virtual Ptr<AgentLoader> createLoaderFor(PPtr<Path> kap,
        const string& className) = 0;
    
    public: virtual void sendFromAre(const k_guid_t& guid, PPtr<KString> opcode,
        PPtr<KValue> payload) = 0;
    
    public: void setNodeCount(k_integer_t n);
    public: PPtr<Path> getHome() const;
    public: PPtr<Path> getDataPath() const;
    public: PPtr<Path> getKapPath() const;
    public: const string& getAppName() const;
    public: void runBundle(PPtr<Path> path) throw(KFException);
    public: void runBundle(PPtr<Bundle> path) throw(KFException);
    public: void unload();
    public: void loadClass(const string& className);
    public: void registerLoader(PPtr<AgentLoader> loader);
    public: PPtr<AgentLoader> getLoaderForClass(const string& className) const;
    public: PPtr<AgentInstance> getInstanceByGuid(const k_guid_t& guid) const;
    
    public: PPtr<AgentInstance> instantiate(PPtr<AgentLoader> loader,
            const string& alias);
    
    public: bool deliverLocally(const k_guid_t& receiver, PPtr<Message> msg);
    public: void deliverLocally(PPtr<Message> msg);
    public: void waitWhileRunning();
    
    
    // Virtual Methods
    public: virtual bool isAlive() const;
    public: virtual bool isActive() const;
    public: virtual void shutdown();
    public: virtual void release();
    public: virtual void signalQuit();
    public: virtual void cleanup();
    public: virtual void handleRuntimeDisconnect(const k_guid_t& rt);
    public: virtual void handleAgentDisconnect(const k_guid_t& agent);
    
    //  Inherited from Runtime //
    public: const k_guid_t& getGuid() const;
    public: k_integer_t getNodeCount() const;
    public: PPtr<Path> getResourcePathForAgent(const Agent* itself) const;
    public: PPtr<Path> getDataPathForAgent(const Agent* itself) const;
    public: const string& getClassNameForAgent(const Agent* itself) const;
    public: const string& getAliasForAgent(const Agent* itself) const;
    public: const k_guid_t& getAgentGuidByAlias(const string& alias) const;
    public: void registerType(PPtr<KType> type);
    public: virtual PPtr<KType> getTypeByHash(const k_longint_t hash) const;
      
    public: void registerMessageFormat(PPtr<KString> opcode,
            PPtr<KType> payloadType);

    public: virtual PPtr<KType> getMessageFormatByHash(const k_longint_t hash) const;
    public: virtual PPtr<KString> getMessageOpCodeForHash(const k_longint_t hash) const;
    
    // Inherited from SerializingStreamer //
    public: void serialize(PPtr<ObjectSerializer> serialzier) const;
    
  };
    
} // namespace are
} // namespace knorba

#endif /* defined(__AgentRuntimeEnvironment__RuntimeBase__) */
