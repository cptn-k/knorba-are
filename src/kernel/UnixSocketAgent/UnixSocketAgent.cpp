//
//  UnixSocketAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/18/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// Unix
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

// KnoRBA
#include <knorba/type/all.h>

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/InputStream.h>
#include <kfoundation/OutputStream.h>
#include <kfoundation/IOException.h>
#include <kfoundation/System.h>
#include <knorba/protocol/UnixSocketClient.h>

// Internal
#include "UnixSocketInputStream.h"
#include "UnixSocketOutputStream.h"

// Self
#include "UnixSocketAgent.h"

#define MAX_QUEUE_SIZE 2
#define SOCK_CAPACITY 4096

//\/ UnixSocketAgent::PTunneling /\////////////////////////////////////////////

UnixSocketAgent::PTunneling::PTunneling(UnixSocketAgent* owner)
: TunnelingServer(owner)
{
  _iAgent = owner;
}


void UnixSocketAgent::PTunneling::onSend(PPtr<KRecord> data) {
  _iAgent->tunnel(data, false);
}


void UnixSocketAgent::PTunneling::onSendBcast(PPtr<KRecord> data) {
  _iAgent->tunnel(data, true);
}


//\/ UnixSocketAgent::ReceiveThread /\/////////////////////////////////////////

UnixSocketAgent::ReceiveThread::ReceiveThread(UnixSocketAgent& owner)
: Thread(owner.getAlias() + " receiver"),
  _owner(owner)
{
  // Nothing;
}


void UnixSocketAgent::ReceiveThread::run() {
  _owner.receiveLoop();
}


//\/ UnixSocketAgent::WatchdogThread /\////////////////////////////////////////

UnixSocketAgent::WatchdogThread::WatchdogThread(UnixSocketAgent& owner)
: Thread(owner.getAlias() + " watchdog"),
  _owner(owner)
{
  // Nothing;
}


void UnixSocketAgent::WatchdogThread::run() {
  _owner.watchdogLoop();
}


//\/ UnixSocketAgent /\////////////////////////////////////////////////////////

// --- STATIC FIELDS --- //

SPtr<KRecordType> UnixSocketAgent::HELLO_T;

const k_octet_t UnixSocketAgent::CODE_HELLO        = 1;
const k_octet_t UnixSocketAgent::CODE_PING         = 2;
const k_octet_t UnixSocketAgent::CODE_TUNNEL       = 4;
const k_octet_t UnixSocketAgent::CODE_TUNNEL_BCAST = 5;

const int UnixSocketAgent::WATCHDOG_N_RETRIES = 5;
const int UnixSocketAgent::WATCHDOG_INTERVAL = 1000;


// --- STATIC METHDOS --- //

PPtr<KRecordType> UnixSocketAgent::hello_t() {
  if(HELLO_T.isNull()) {
    HELLO_T = new KRecordType("knorba.unix-dgram-socket.hello");
    HELLO_T->addField("kernel", KType::GUID)
           ->addField("runtime", KType::GUID)
           ->addField("address", KType::STRING);
    HELLO_T.setSelfDestruct();
  }
  
  return HELLO_T;
}


// --- (DE)CONSTRUCTORS --- //

UnixSocketAgent::UnixSocketAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid),
  _pTunneling(this)
{
  setPassive();
  
  _stopFlag = false;
  _inputSocket = new UnixSocketInputStream();
  _receiveThread = new ReceiveThread(*this);
  _watchdogThread = new WatchdogThread(*this);
  _routeTable = new ManagedArray<RouteTableItem>();
  
  registerHandler((handler_t)&UnixSocketAgent::handleOpAddClient,
      UnixSocketClient::OP_ADD_CONNECTION);
  
  registerHandler((handler_t)&UnixSocketAgent::handleOpSetAddress,
      UnixSocketClient::OP_SET_ADDRESS);
}


UnixSocketAgent::~UnixSocketAgent() {
  // Nothing
}


// --- METHODS --- //

void UnixSocketAgent::watchdogLoop() {
  ALOG << "Watchdog thread started." << EL;
  
  while(!_stopFlag) {
    for(int i = WATCHDOG_INTERVAL/400; i >= 0 && !_stopFlag; i--) {
      System::sleep(400 + rand()%100);
    }
    
    for(int i = _routeTable->getSize() - 1; i >= 0; i--) {
      PPtr<RouteTableItem> route = _routeTable->at(i);
      if(route->nRetries == 0) {
        route->nRetries = -1;
        ALOG << "Remote kernel not responding: " << route->kernel << EL;
        _pTunneling.opRemoveRoute(route->kernel);
      } else if(route->nRetries > 0){
        route->nRetries--;
        netSend(route->path, CODE_PING, new KGlobalUid(_pTunneling.getClient()), i);
      }
    } // for(i)
  } // while(!_stopFlag)
  
  ALOG << "Watchdog thread terminated." << EL;
}


void UnixSocketAgent::receiveLoop() {
  ALOG << "Listener thread started." << EL;
  
  while(!_stopFlag) {
    receive();
  }
  
  ALOG << "Listener thread terminated." << EL;
}


void UnixSocketAgent::receive() {
  _inputSocket->listen();
  
  if(!_inputSocket->isBound()) {
    return;
  }
  
  Ptr<KLongint> sizeVal = new KLongint();
  sizeVal->readFromBinaryStream(_inputSocket.AS(InputStream));

  int opcode = _inputSocket->read();
  
  switch (opcode) {
    case -1:
      ALOG_ERR << "Not enough bytes to read." << EL;
      break;
      
    case CODE_HELLO:
      receiveHello(_inputSocket.AS(InputStream));
      break;
      
    case CODE_PING:
      receivePing(_inputSocket.AS(InputStream));
      break;
      
    case CODE_TUNNEL:
      receiveTunnel(_inputSocket.AS(InputStream), false);
      break;
      
    case CODE_TUNNEL_BCAST:
      receiveTunnel(_inputSocket.AS(InputStream), true);
      break;
  };
  
  _inputSocket->close();
}


void UnixSocketAgent::receiveHello(PPtr<InputStream> input) {
  Ptr<KRecord> data = new KRecord(hello_t());
  data->readFromBinaryStream(input);
  int index = checkAndRegisterClient(data);
  if(index >= 0) {
    ALOG << "Remote kernel registered: " << *data << EL;
    
    _pTunneling.opAddRoute(data->getGuid(HELLO_T_RUNTIME),
        data->getGuid(HELLO_T_KERNEL));
    
    sayHello(_routeTable->at(index)->path);
  } else {
    // ALOG << "Ignored hello from registered kernel: " << *data<< EL;
  }
}


void UnixSocketAgent::receivePing(PPtr<InputStream> input) {
  Ptr<KGlobalUid> sender = new KGlobalUid();
  sender->readFromBinaryStream(input);
  
  int index = getRouteTableIndexOf(sender->get());
  
  if(index >= 0) {
    _routeTable->at(index)->nRetries = WATCHDOG_N_RETRIES;
  } else {
    ALOG_WRN << "Received ping from unregistered kernel: " << sender->get()
        << EL;
  }
}


void UnixSocketAgent::receiveTunnel(PPtr<InputStream> input, bool bcast) {
  Ptr<KRecord> data = new KRecord(TunnelingProtocol::message_t());
  data->setRuntime(getRuntime());
  
  try {
    data->readFromBinaryStream(input);
  } catch(KFException e) {
    ALOG_WRN << e << EL;
    return;
  }
  
  if(bcast) {
    _pTunneling.opReceiveBcast(data);
  } else {
    _pTunneling.opReceive(data);
  }
}


int UnixSocketAgent::checkAndRegisterClient(PPtr<KRecord> helloMsg) {
  k_guid_t sender = helloMsg->getGuid(HELLO_T_KERNEL);
  
  int index = getRouteTableIndexOf(sender);
  if(index >= 0) {
    return -1;
  }
  
  Ptr<RouteTableItem> item = new RouteTableItem();
  item->kernel = sender;
  item->path = new Path(helloMsg->getString(HELLO_T_ADDRESS)->getUtf8CStr());
  item->nRetries = WATCHDOG_N_RETRIES;
  
  index = _routeTable->getSize();
  _routeTable->push(item);
  
  return index;
}


int UnixSocketAgent::getRouteTableIndexOf(const k_guid_t& guid) const {
  for(int i = _routeTable->getSize() - 1; i >= 0; i--) {
    if(KGlobalUid::areOnTheSameNode(guid, _routeTable->at(i)->kernel)) {
      return i;
    }
  }
  
  return -1;
}


void UnixSocketAgent::sayHello(PPtr<Path> target) {
  Ptr<KRecord> rec = new KRecord(hello_t());
  rec->setGuid(HELLO_T_KERNEL, _pTunneling.getClient());
  rec->setGuid(HELLO_T_RUNTIME, getRuntime().getGuid());
  rec->getString(HELLO_T_ADDRESS)->set(_inputSocket->getPath()->getString());
  Ptr<UnixSocketOutputStream> output = new UnixSocketOutputStream();
  netSend(target, CODE_HELLO, rec.AS(KValue), -1);
}


void UnixSocketAgent::tunnel(PPtr<KRecord> msg, bool bcast) {
  if(bcast) {
    for(int i = _routeTable->getSize() - 1; i >= 0; i--) {
      if(_routeTable->at(i)->nRetries >= 0) {
        netSend(_routeTable->at(i)->path, CODE_TUNNEL_BCAST, msg.AS(KValue), i);
      }
    }
  } else {
    k_guid_t target = msg->getGuid(0);
    int index = getRouteTableIndexOf(target);
    if(index == -1) {
      ALOG_ERR << "No route to " << target << EL;
    } else if(_routeTable->at(index)->nRetries >= 0) {
      netSend(_routeTable->at(index)->path, CODE_TUNNEL, msg.AS(KValue), index);
    } else {
      ALOG_WRN << "Target is dead: " << target << EL;
    }
  }
}


void UnixSocketAgent::netSend(PPtr<Path> target, k_octet_t opcode,
    PPtr<KValue> msg, int routeTableIndex)
{
  Ptr<UnixSocketOutputStream> output = new UnixSocketOutputStream();
  
  try {
    output->connect(target);
  } catch(IOException& e) {
    ALOG_WRN << e.getMessage() << EL;
    output->close();
    
    if(routeTableIndex >= 0) {
      PPtr<RouteTableItem> route = _routeTable->at(routeTableIndex);
      ALOG << "Remote kernel disconnected: " << route->kernel << EL;
      _pTunneling.opRemoveRoute(route->kernel);
      route->nRetries = -1;
    }
    
    return;
  }
  
  kf_int32_t dataSize = (kf_int32_t)msg->getTotalSizeInOctets();
  kf_int32_t totalSize = dataSize + 9;
  
  Ptr<KLongint> sizeVal = new KLongint(totalSize);
  sizeVal->writeToBinaryStream(output.AS(OutputStream));
  output->write(opcode);
  msg->writeToBinaryStream(output.AS(OutputStream));
  
  output->close();
}


void UnixSocketAgent::handleOpSetAddress(PPtr<Message> msg) {
  if(_receiveThread->isRunning()) {
    ALOG_WRN << "Could not set address. Agent is already active at "
        << *_inputSocket->getPath() << EL;
    return;
  }

  PPtr<KString> address = msg->getPayload().AS(KString);
  _inputSocket->bind(new Path(address->toUtf8String()));
  
  _stopFlag = false;
  _receiveThread->start();
  _watchdogThread->start();
  
  ALOG << "Active at " << *address << EL;
}


void UnixSocketAgent::handleOpAddClient(PPtr<Message> msg) {
  PPtr<KString> address = msg->getPayload().AS(KString);
  LOG << "Saying hello to " << *address << EL;
  sayHello(new Path(address->toUtf8String()));
}


bool UnixSocketAgent::isAlive() {
  return Agent::isAlive() || _receiveThread->isRunning()
      || _watchdogThread->isRunning();
}


void UnixSocketAgent::finalize() {
  _stopFlag = true;
  _inputSocket->unbind();
  
  Agent::finalize();
}


//\/ UnixSocketAgentLoader /\//////////////////////////////////////////////////

KF_EXPORT
void init(Runtime& rt) {
  TunnelingProtocol::init(rt);
  UnixSocketClient::init(rt);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new UnixSocketAgent(rt, guid);
}
