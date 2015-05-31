//
//  ClusterSetupProtocol.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 4/28/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__ClusterSetupProtocol__
#define __AgentRuntimeEnvironment__ClusterSetupProtocol__

#include <knorba/Protocol.h>
#include <knorba/type/all.h>

namespace knorba {
namespace protocol {
  
  using namespace type;
  
  class ClusterSetupProtocol : public Protocol {
  
  // --- NESTED TYPES --- //
    
  private: struct ClusterTableItem {
    public: k_guid_t guid;
    public: k_longint_t orderToken;
    public: k_integer_t rank;
  };
    
    
  // --- STATIC FIELDS --- //
    
    private: static SPtr<KRecordType> HELLO_T;
    public: static const SPtr<KString> OP_SET;
    public: static const SPtr<KString> OP_HELLO;
    
    
  // --- STATIC METHODS --- //
    
    public: static SPtr<KRecordType> hello_t();
    public: static void init(Runtime& rt);
    
    
  // --- FIELDS --- //
    
    private: k_integer_t _ordinal;
    private: k_integer_t _rank;
    private: bool _isHead;
    private: bool _isTail;
    private: bool _isRankFixed;
    private: Ptr<KString> _clusterName;
    private: Ptr<Group> _cluster;
    private: Ptr< Array<ClusterTableItem> > _clusterTable;
    
    
  // --- (DE)CONSTRUCTORS --- //
    
    public: ClusterSetupProtocol(Agent* owner);
    
    
  // --- METHODS --- //
    
    private: int getIndexForGuid(const k_guid_t& guid) const;
    private: void printCluster();
    private: void refresh();
    public: void fixRank();
    public: bool isHead() const;
    public: PPtr<Group> getCluster() const;
    public: k_integer_t getRank() const;
    public: void addNode(const k_guid_t& remoteKernel);
    public: void removeNode(const k_guid_t& remoteKernel);
    public: int getNumberOfNodes() const;
    public: const k_guid_t& getGuidForKernelAtIndex(int index) const;
    public: k_integer_t getRankForKernelAtIndex(int index) const;
    public: void handleOpSetup(PPtr<Message> msg);
    public: void handleOpSetCluster(PPtr<Message> msg);
    public: void handleOpHello(PPtr<Message> msg);
    
  };

} // namespace protocol
} // namespace knorba

#endif /* defined(__AgentRuntimeEnvironment__ClusterSetupProtocol__) */