//
//  VirtualRuntime.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/20/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__VirtualRuntime__
#define __AgentRuntimeEnvironment__VirtualRuntime__

// Super
#include "RuntimeBase.h"

namespace knorba {
namespace are {

  class KernelRuntime;
  
  class VirtualRuntime : public RuntimeBase {
    
  // --- FIELDS --- //
    
    private: KernelRuntime& _owner;
    private: k_guid_t _console;
    private: bool _closing;
    private: bool _readyToDelete;

        
  // --- (DE)CONSTRUCTORS --- //
    
    public: VirtualRuntime(KernelRuntime& owner, PPtr<Path> knorbaHome,
        const k_guid_t& guid);
    
    public: ~VirtualRuntime();
    
    
  // --- METHODS --- //
    
    private: Logger::Stream& log(const Logger::level_t = Logger::L3) const;
    private: void deliver(const k_guid_t& receiver, PPtr<Message> msg);
    public: void shutdown();
    public: void setConsole(const k_guid_t& console);
    public: bool isReadyToDelete() const;
    
    // Inherited from RuntimeBase
    public: Ptr<AgentLoader> createLoaderFor(PPtr<Path> kap, const string& className);
    public: void sendFromAre(const k_guid_t& guid, PPtr<KString> opcode,
            PPtr<KValue> payload);
    
    public: bool isAlive() const;
    public: void signalQuit();
    public: virtual void handleAgentDisconnect(const k_guid_t& agent);
    
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
    
  };
  
} // are
} // knorba

#endif /* defined(__AgentRuntimeEnvironment__VirtualRuntime__) */
