//
//  KernelRuntime.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/20/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__KernelRuntime__
#define __AgentRuntimeEnvironment__KernelRuntime__

// KFoundation
#include <kfoundation/ObjectPoolMemoryManager.h>
#include <kfoundation/ManagedArrayDecl.h>

// Super
#include "RuntimeBase.h"

namespace kfoundation {
  class FileOutputStream;
}

namespace knorba {
  class AgentLoader;
}

namespace knorba {
namespace type {
  class KRecord;
  class KRecordType;
}
}


namespace knorba {
namespace are {
  
  class KernelAgent;
  class VirtualRuntime;
  class UnixSocketInputStream;
  
  class KernelRuntime: public RuntimeBase {
    
  // --- NESTED TYPES --- //
    
    private: class RunParam : public ManagedObject {
      public: PPtr<VirtualRuntime> rt;
      public: Ptr<Bundle> bundle;
    };
    
    
  // --- STATIC FIELDS --- //
    
    private: static SPtr<KRecordType> PID_FILE_T;
    
    
  // --- STATIC METHODS --- //
    
    private: static PPtr<KRecordType> pid_file_t();
    private: static k_guid_t generateRandomGuid();
    
    
  // --- FIELDS --- //
    
    private: bool _closing;
    private: KernelAgent* _kernelAgent;
    private: Ptr<RunParam> _runParam;
    private: ObjectPoolMemoryManager<Message> _messagePool;
    private: Ptr<FileOutputStream> _pidFile;
    private: Ptr< ManagedArray<VirtualRuntime> > _virtualRuntimes;

    
  // --- (DE)CONSTRUCTORS --- //
  
    public: KernelRuntime(PPtr<Path> knorbaHome);
    public: ~KernelRuntime();
    
    
  // --- METHODS --- //
    
    public: Ptr<KRecord> getPidFileContents() const;
    public: void makePidFile() const;
    public: void releasePidFile() const;
    public: void kernelRun();
    public: void kernelShutdown(int pid);
    public: void kernelRestart(int pid);
    public: void kernelPrintStatus(int pid);
    public: void kernelRunBundle(int pid, const string& bundle);
    public: void runBundleInMainThread(PPtr<VirtualRuntime> rt, PPtr<Bundle> b);
    public: PPtr<VirtualRuntime> newVirtualRuntime(const k_guid_t& guid);
    public: Ptr<Message> newMessage();
    public: void deliver(const k_guid_t& receiver, PPtr<Message> msg);
    
    private: bool deliverToVirtualRuntimes(const k_guid_t& receiver,
        PPtr<Message> msg);

    public: void deliverToAll(PPtr<Message> msg);

    
    // Inherited from RuntimeBase
    public: Ptr<AgentLoader> createLoaderFor(PPtr<Path> kap,
        const string& className);
    
    public: void sendFromAre(const k_guid_t& guid, PPtr<KString> opcode,
        PPtr<KValue> payload);
    
    public: bool isAlive() const;
    public: bool isActive() const;
    public: void cleanup();
    public: void shutdown();
    public: void release();
    public: void handleRuntimeDisconnect(const k_guid_t& rt);
    public: void handleAgentDisconnect(const k_guid_t& agent);

    public: PPtr<KType> getTypeByHash(const k_longint_t hash) const;
    public: PPtr<KType> getMessageFormatByHash(const k_longint_t hash) const;
    public: PPtr<KString> getMessageOpCodeForHash(const k_longint_t hash) const;

    //  Inherited from RuntimeBase::Runtime //
    public: const k_guid_t& getConsoleGuid() const;
    public: bool isHead() const;
    
    public: void send(const k_guid_t& sender, const k_guid_t& receiver,
            const k_longint_t opcode, PPtr<KValue> content, const k_integer_t tid);
      
    public: void send(const k_guid_t& sender, PPtr<Group> receivers,
            const k_longint_t opcode, PPtr<KValue> content, const k_integer_t tid);
      
    public: void sendToAll(const k_guid_t& sender, const k_longint_t opcode,
            PPtr<KValue> content, const k_integer_t tid);
      
    public: void sendToLocals(const k_guid_t& sender, const k_longint_t opcode,
            PPtr<KValue> content, const k_integer_t tid);
    
    // Inherited from RuntimeBase::SerializingStreamer //
    public: void serialize(PPtr<ObjectSerializer> serializer) const;
  };

  
} // namespace are
} // namespace knorba

#endif /* defined(__AgentRuntimeEnvironment__KernelRuntime__) */
