//
//  ClusterSetupProtocol.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 4/28/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/RangeIterator.h>
#include <kfoundation/Path.h>

//KnoRBA
#include <knorba/Runtime.h>
#include <knorba/Agent.h>
#include <knorba/Group.h>

// Internal
#include "ClusterSetupProtocol.h"

namespace knorba {
namespace protocol {
  
// --- ClusterSetupProtocol --- //
  
  SPtr<KRecordType> ClusterSetupProtocol::HELLO_T;
  
  const SPtr<KString> ClusterSetupProtocol::OP_SET
      = KS("knorba.kernel.cluster.set");
  
  const SPtr<KString> ClusterSetupProtocol::OP_HELLO
      = KS("knorba.kernel.cluster.hello");
  
  
// --- STATIC METHODS --- //
  
  SPtr<KRecordType> ClusterSetupProtocol::hello_t() {
    if(HELLO_T.isNull()) {
      HELLO_T = new KRecordType("knorba.kernel.cluster.Hello");
      HELLO_T->addField("clusterId", KType::LONGINT)
             ->addField("birthTime", KType::LONGINT)
             ->addField("rank", KType::INTEGER);
    }
    
    return HELLO_T;
  }
  
  
  void ClusterSetupProtocol::init(Runtime& rt) {
    rt.registerMessageFormat(OP_HELLO, hello_t().AS(KType));
    rt.registerMessageFormat(OP_SET, KType::STRING);
  }
  
  
// --- (DE)CONSTRUCTORS --- //
  
  ClusterSetupProtocol::ClusterSetupProtocol(Agent* owner)
  : Protocol(owner)
  {
    _isHead = true;
    _isTail = true;
    _cluster = new Group();
    _clusterTable = new Array<ClusterTableItem>();
    
    ClusterTableItem& myItem = _clusterTable->push();
    myItem.guid = _agent->getGuid();
    myItem.orderToken = 1000000 + rand()%1000000;
    myItem.rank = -1;
    
    _ordinal = 0;
    _isRankFixed = false;
    
    registerHandler((phandler_t)&ClusterSetupProtocol::handleOpSetCluster, OP_SET);
    registerHandler((phandler_t)&ClusterSetupProtocol::handleOpHello, OP_HELLO);
  }
  
  
// --- METHODS --- //
  
  int ClusterSetupProtocol::getIndexForGuid(const k_guid_t& guid) const {
    for(int i = _clusterTable->getSize() - 1; i >= 0; i--) {
      if(_clusterTable->at(i).guid == guid) {
        return i;
      }
    }
    return -1;
  }
  
  
  void ClusterSetupProtocol::printCluster() {
    int s = _clusterTable->getSize();
    Logger::Stream& stream = PLOG << "clusterTable:";
    for(int i  = 0; i < s; i++) {
      ClusterTableItem& item = _clusterTable->at(i);
      if(i == _ordinal) {
        stream << "\n  > ";
      } else {
        stream << "\n    ";
      }
      stream << i << ". "  << item.guid << ", " << item.rank << ", "
          << item.orderToken;
    }
    stream << EL;
  }
  
  
  void ClusterSetupProtocol::refresh() {
    _ordinal = getIndexForGuid(_agent->getGuid());
    
    _isHead = (_ordinal == 0);
    _isTail = (_ordinal == _clusterTable->getSize() - 1);
    
    printCluster();
  }
  
  
  void ClusterSetupProtocol::fixRank() {
    if(_isRankFixed) {
      return;
    }
    
    int nextRank = 0;
    int s = _clusterTable->getSize();
    
    for(int i = 0; i < s; i++) {
      ClusterTableItem& item = _clusterTable->at(i);
      if(item.rank == -1) {
        item.rank = nextRank;
        item.orderToken = nextRank;
        nextRank++;
      } else {
        nextRank = item.rank + 1;
      }
    }
    
    _rank = _clusterTable->at(_ordinal).rank;
    _isRankFixed = true;
    
    PLOG << "Assigned rank " << _rank << " in cluster \"" << *_clusterName
      << "\"" << EL_CON;
  }
  
  
  bool ClusterSetupProtocol::isHead() const {
    return _isHead;
  }
  
  
  PPtr<Group> ClusterSetupProtocol::getCluster() const {
    return _cluster;
  }
  
  
  k_integer_t ClusterSetupProtocol::getRank() const {
    return _rank;
  }
  
  
  void ClusterSetupProtocol::addNode(const k_guid_t& remoteKernel) {
    if(_clusterName.isNull()) {
      LOG_WRN << "Cluster name is null" << EL;
      return;
    }
    
    ClusterTableItem& myItem = _clusterTable->at(_ordinal);
    
    Ptr<KRecord> hello = new KRecord(hello_t());
    hello->setLongint(0, _clusterName->getHashCode());
    hello->setLongint(1, myItem.orderToken);
    hello->setInteger(2, myItem.rank);
    
    _agent->send(remoteKernel, OP_HELLO, hello.AS(KValue));
  }

  
  void ClusterSetupProtocol::removeNode(const k_guid_t& remoteKernel) {
    int index = getIndexForGuid(remoteKernel);
    if(index >= 0) {
      _cluster->remove(remoteKernel);
      _clusterTable->remove(index);
      refresh();
    }
  }
  
  
  int ClusterSetupProtocol::getNumberOfNodes() const {
    return _clusterTable->getSize();
  }
  
  
  const k_guid_t& ClusterSetupProtocol::getGuidForKernelAtIndex(int index) const {
    return _clusterTable->at(index).guid;
  }
  
  
  k_integer_t ClusterSetupProtocol::getRankForKernelAtIndex(int index) const {
    return _clusterTable->at(index).rank;
  }

  
  void ClusterSetupProtocol::handleOpSetup(PPtr<Message> msg) {
    PLOG_ERR << "handleOpSetup()" << EL;
  }
  
  
  void ClusterSetupProtocol::handleOpSetCluster(PPtr<Message> msg) {
    _clusterName = new KString();
    _clusterName->set(msg->getPayload());
  }
  
  
  void ClusterSetupProtocol::handleOpHello(PPtr<Message> msg) {
    if(_clusterName.isNull()) {
      return;
    }
    
    PPtr<KRecord> data = msg->getPayload().AS(KRecord);
    if(data->getLongint(0) != _clusterName->getHashCode()) {
      return;
    }
    
    _cluster->add(msg->getSender());
    
    ClusterTableItem newItem;
    newItem.guid = msg->getSender();
    newItem.rank = data->getInteger(2);
    
    if(newItem.rank >= 0) {
      newItem.orderToken = newItem.rank;
    } else {
      newItem.orderToken = data->getLongint(1);
    }
    
    int index = -1;
    for(int i = _clusterTable->getSize() - 1; i >= 0; i--) {
      if(_clusterTable->at(i).orderToken > newItem.orderToken) {
        index = i;
      } else {
        break;
      }
    }
    
    if(index == -1) {
      _clusterTable->push(newItem);
    } else {
      _clusterTable->insert(index, newItem);
    }
    
    refresh();
  }
  
} // namespace protocol
} // namespace knorba