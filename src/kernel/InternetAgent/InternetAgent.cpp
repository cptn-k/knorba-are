//
//  InternetAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/30/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/InternetAddress.h>
#include <kfoundation/InternetInputStream.h>
#include <kfoundation/InternetOutputStream.h>
#include <kfoundation/Thread.h>
#include <kfoundation/Mutex.h>
#include <kfoundation/Condition.h>

// KnoBRA
#include <knorba/type/all.h>
#include <knorba/protocol/TunnelingServer.h>

// Self
#include "InternetAgent.h"

#undef ADLOG
#define ADLOG(X) KF_NOP


//\/ InternetAgent::TunnelingServerImpl /\/////////////////////////////////////

InternetAgent::PTunneling::PTunneling(InternetAgent* owner)
: TunnelingServer(owner)
{
  _iAgent = owner;
}


void InternetAgent::PTunneling::onSend(PPtr<KRecord> data) {
  _iAgent->tunnel(data, false);
}


void InternetAgent::PTunneling::onSendBcast(PPtr<KRecord> data) {
  _iAgent->tunnel(data, true);
}


//\/ InternetAgent::ListenerThread /\//////////////////////////////////////////

InternetAgent::ReceiverThread::ReceiverThread(InternetAgent& owner)
: Thread(owner.getAlias() + " receiver"),
  _owner(owner)
{
  // Nothing;
}


void InternetAgent::ReceiverThread::run() {
  _owner.receiverLoop();
}


//\/ InternetAgent::SenderThread /\////////////////////////////////////////////

InternetAgent::SenderThread::SenderThread(InternetAgent& owner)
: Thread(owner.getAlias() + " sender"),
  _owner(owner)
{
  // Nothing;
}


void InternetAgent::SenderThread::run() {
  _owner.senderLoop();
}


//\/ InternetAgent::WatchdogThread /\//////////////////////////////////////////

InternetAgent::WatchdogThread::WatchdogThread(InternetAgent& owner)
: Thread(owner.getAlias() + " watchdog"),
  _owner(owner)
{
  // Nothing;
}


void InternetAgent::WatchdogThread::run() {
  _owner.watchdogLoop();
}


//\/ InternetAgent /\//////////////////////////////////////////////////////////

// --- STATIC FIELDS --- //

SPtr<KRecordType> InternetAgent::HELLO_T;

const k_octet_t InternetAgent::CODE_HELLO        = 1;
const k_octet_t InternetAgent::CODE_PING         = 2;
const k_octet_t InternetAgent::CODE_TUNNEL       = 3;
const k_octet_t InternetAgent::CODE_TUNNEL_BCAST = 4;

const SPtr<KString> InternetAgent::OP_ADD_CONNECTION
    = KS("knorba.internet.add-connection");

const SPtr<KString> InternetAgent::OP_SET_ADDRESS
    = KS("knorba.internet.set-address");


// --- STATIC METHODS --- //

PPtr<KRecordType> InternetAgent::hello_t() {
  if(HELLO_T.isNull()) {
    HELLO_T = new KRecordType("konrba.internet.Hello");
    HELLO_T->addField("kernelGuid", KType::GUID)
      ->addField("runtimeGuid", KType::GUID)
      ->addField("address", KType::STRING);
  }
  
  return HELLO_T;
}


// --- (DE)CONSTRUCTORS --- //

InternetAgent::InternetAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid),
  _pTunneling(this)
{
  setPassive();
  
  _stopFlag = false;
  _queueHead = 0;
  _queueTail = 0;
  _queueCount = 0;
  
  _senderThread = new SenderThread(*this);
  _receiverThread = new ReceiverThread(*this);
  _watchDogThread = new WatchdogThread(*this);
  
  _inputSocket = new InternetInputStream();
  _routeTable = new ManagedArray<RouteTableItem>();
  
  registerHandler((handler_t)&InternetAgent::handleOpSetHost, OP_SET_ADDRESS);
  
  registerHandler((handler_t)&InternetAgent::handleOpAddConnection,
      OP_ADD_CONNECTION);
}


// --- METHODS --- //

PPtr<InternetAgent::RouteTableItem>
InternetAgent::findRouteToKernel(const k_guid_t& guid) {
  for(int i = _routeTable->getSize() - 1; i >= 0; i--) {
    if(guid == _routeTable->at(i)->kernel) {
      return _routeTable->at(i);
    }
  }
  
  return NULL;
}


// Threads //

void InternetAgent::senderLoop() {
  ALOG << "Sender thread started." << EL;
  
  while(!_stopFlag) {
    if(_queueHead == _queueTail) {
      _newMessageCond.block();
    }
    
    while(_queueHead != _queueTail && !_stopFlag) {
      MessageQueueItem& item = _messageQueue[_queueTail];
      
      if(item.opcode == CODE_TUNNEL_BCAST) {
        for(int i = _routeTable->getSize() - 1; i >=0 ; i--) {
          item.route = _routeTable->at(i);
          netSend(item);
        }
      } else {
        netSend(item);
      }
    
      item.payload = NULL;

      KF_SYNCHRONIZED(_messageQueueMutex,
        _queueTail++;
        _queueCount--;
        if(_queueTail == MESSAGE_QUEUE_SIZE) {
          _queueTail = 0;
        }
      ) // KF_SYNCHRONIZED
    }
  }
  
  ALOG << "Sender thread terminated." << EL;
}


void InternetAgent::receiverLoop() {
  ALOG << "Receiver thread started." << EL;
  
  while(!_stopFlag) {
    netReceive();
  }
  
  ALOG << "Receiver thread terminated." << EL;
}


void InternetAgent::watchdogLoop() {
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
        enqueue(CODE_PING, route, new KGlobalUid(_pTunneling.getClient()));
      }
    } // for(i)
  } // while(!_stopFlag)
  
  ALOG << "Watchdog thread terminated." << EL;
}


// Send and Receive //

void InternetAgent::netSend(MessageQueueItem& messageItem) {
  if(messageItem.route->nRetries <= 0) {
    return;
  }
  
  PPtr<InternetOutputStream> os = messageItem.route->os;
  
  try {
    os->connect();
  } catch(IOException& e) {
    os->close();
    
    ALOG_WRN << e.getMessage() << EL;
    
    messageItem.route->nRetries = -1;
    
    const k_guid_t& kernel = messageItem.route->kernel;
    
    if(!(kernel == KGlobalUid::zero())) {
      ALOG << "Remote kernel disconnected: " << kernel << EL;
      _pTunneling.opRemoveRoute(kernel);
    }
    
    return;
  }
  
  kf_int32_t dataSize = (kf_int32_t)messageItem.payload->getTotalSizeInOctets();
  kf_int32_t totalSize = dataSize + 9;
  
  Ptr<KLongint> sizeVal = new KLongint(totalSize);
  
  try {
    sizeVal->writeToBinaryStream(os.AS(OutputStream));
    os->write(messageItem.opcode);
    messageItem.payload->writeToBinaryStream(os.AS(OutputStream));
    os->close();
    
    if(totalSize != os->getNSentOctets()) {
      ALOG_ERR << "Calculated size (" << totalSize << ") and actual size ("
          << os->getNSentOctets() << ") mismatch for message "
          << *messageItem.payload << EL;
    }    
  } catch(IOException& e) {
    os->close();
    ALOG_ERR << e.getMessage() << EL;
    ALOG << e << EL;
  }
}


void InternetAgent::netReceive() {
  _inputSocket->listen();
  
  if(!_inputSocket->isBound()) {
    return;
  }
  
  Ptr<KLongint> sizeVal = new KLongint();
  
  try {
    sizeVal->readFromBinaryStream(_inputSocket.AS(InputStream));
  } catch(IOException& e) {
    ALOG_ERR << e.getMessage() << EL;
    ALOG << e << EL;
    _inputSocket->close();
    return;
  }
  
  int opcode = _inputSocket->read();
  
  switch (opcode) {
    case -1:
      ALOG_ERR << "Not enough bytes to read." << EL;
      break;
      
    case CODE_HELLO:
      receiveHello();
      break;
      
    case CODE_PING:
      receivePing();
      break;
      
    case CODE_TUNNEL:
      receiveTunnel(false);
      break;
      
    case CODE_TUNNEL_BCAST:
      receiveTunnel(true);
      break;
  };
  
  _inputSocket->close();
  
}


void InternetAgent::receiveHello() {
  Ptr<KRecord> data = new KRecord(hello_t());
  data->readFromBinaryStream(_inputSocket.AS(InputStream));
  PPtr<RouteTableItem> route = checkAndRegisterClient(data);
  if(!route.isNull()) {
    _pTunneling.opAddRoute(data->getGuid(1), data->getGuid(0));
    sayHello(route);
  } else {
    // ALOG << "Ignored hello from registered client: " << *data << EL;
  }
}


void InternetAgent::receiveTunnel(bool bcast) {
  Ptr<KRecord> data = new KRecord(TunnelingProtocol::message_t());
  data->setRuntime(getRuntime());
  
  try {
    data->readFromBinaryStream(_inputSocket.AS(InputStream));
  } catch(KFException e) {
    ALOG_WRN << e << EL;
    return;
  }
  
  ADLOG("Received: " << *data);
  
  if(bcast) {
    _pTunneling.opReceiveBcast(data);
  } else {
    _pTunneling.opReceive(data);
  }
}


void InternetAgent::receivePing() {
  Ptr<KGlobalUid> sender = new KGlobalUid();
  sender->readFromBinaryStream(_inputSocket.AS(InputStream));
  
  PPtr<RouteTableItem> route = findRouteToKernel(sender->get());
  
  if(!route.isNull()) {
    route->nRetries = WATCHDOG_N_RETRIES;
  } else {
    ALOG_WRN << "Received ping from unregistered kernel: " << sender->get()
        << EL;
  }
}


PPtr<InternetAgent::RouteTableItem>
InternetAgent::checkAndRegisterClient(PPtr<KRecord> data)
{
  InternetAddress addr(data->getString(HELLO_T_ADDRESS)->toUtf8String());
  
  Ptr<RouteTableItem> route;
  for(int i = _routeTable->getSize() - 1; i >= 0; i--) {
    if(_routeTable->at(i)->os->getAddress().equals(addr)) {
      route = _routeTable->at(i);
      break;
    }
  }
  
  if(route.isNull()) {
    route = new RouteTableItem();
    try {
      route->os = new InternetOutputStream(addr);
      _routeTable->push(route);
    } catch(KFException e) {
      ALOG_ERR << e.getMessage() << EL;
      ALOG << e << EL;
      return NULL;
    }
  } else if(route->kernel == data->getGuid(HELLO_T_KERNEL)) {
    return NULL;
  }
  
  route->runtime = data->getGuid(HELLO_T_RUNTIME);
  route->kernel = data->getGuid(HELLO_T_KERNEL);
  route->nRetries = WATCHDOG_N_RETRIES;
  
  ALOG << "Registered route to kernel: " << route->kernel << EL;
  
  return route;
}


void InternetAgent::sayHello(PPtr<RouteTableItem> route) {
  Ptr<KRecord> rec = new KRecord(hello_t());
  rec->setGuid(HELLO_T_KERNEL, _pTunneling.getClient());
  rec->setGuid(HELLO_T_RUNTIME, getRuntime().getGuid());
  rec->getString(HELLO_T_ADDRESS)->set(_inputSocket->getAddress().toString());

  ALOG << "Saying hello to " << route->os->getAddress() << EL;
  
  enqueue(CODE_HELLO, route, rec.AS(KValue));
}


void InternetAgent::tunnel(PPtr<KRecord> msg, bool bcast) {
  if(bcast) {
    enqueue(CODE_TUNNEL_BCAST, NULL, msg.AS(KValue));
  } else {
    
    PPtr<RouteTableItem> route = findRouteToKernel(
        msg->getGuid(TunnelingProtocol::MESSAGE_T_REMOTE_KERNEL));
    
    if(route.isNull()) {
      ALOG_ERR << "No route. Message could not be sent: " << *msg << EL;
    } else {
      enqueue(CODE_TUNNEL, route, msg.AS(KValue));
    }
  }
}


void InternetAgent::enqueue(k_octet_t opcode, PPtr<RouteTableItem> route,
    PPtr<KValue> payload)
{
  if(_queueCount == MESSAGE_QUEUE_SIZE) {
    ALOG_ERR << "Sender queue is full. Waiting." << EL;
    while (_queueCount == MESSAGE_QUEUE_SIZE) {
      System::sleep(100);
    }
  }
  
  KF_SYNCHRONIZED(_messageQueueMutex,
    MessageQueueItem& item = _messageQueue[_queueHead];

    item.opcode = opcode;
    item.route = route;
    item.payload = payload;

    _queueHead++;
    _queueCount++;
    if(_queueHead == MESSAGE_QUEUE_SIZE) {
      _queueHead = 0;
    }
  )
  
  _newMessageCond.release();
}


// Message Handlers //

void InternetAgent::handleOpSetHost(PPtr<Message> msg) {
  if(_inputSocket->isBound()) {
    ALOG_ERR << "Host is already active with addess "
        << _inputSocket->getAddress() << EL;
    return;
  }
  
  try {
    InternetAddress addr(msg->getPayload().AS(KString)->toUtf8String());
    
    if(addr.getPort() == -1) {
      addr = addr.copyWithPort(DEFAULT_PORT);
    }
    
    _inputSocket->bind(addr);
    
    _senderThread->start();
    _receiverThread->start();
    _watchDogThread->start();
    
    ALOG << "Active at: " << _inputSocket->getAddress() << EL;
  } catch(KFException e) {
    ALOG_ERR << e.getMessage() << EL;
    ALOG << e << EL;
  }
}


void InternetAgent::handleOpAddConnection(PPtr<Message> msg) {
  try {
    InternetAddress addr(msg->getPayload().AS(KString)->toUtf8String());
    
    Ptr<RouteTableItem> route = new RouteTableItem();
    route->os = new InternetOutputStream(addr);
    route->runtime = KGlobalUid::zero();
    route->kernel = KGlobalUid::zero();
    route->nRetries = WATCHDOG_N_RETRIES;
    
    _routeTable->push(route);
    
    sayHello(route);
  } catch(KFException e) {
    ALOG_ERR << e.getMessage() << EL;
    ALOG << e << EL;
  }
}


// Inhertied from Agent //

void InternetAgent::finalize() {
  ALOG << "Finalizing." << EL;
  
  _stopFlag = true;
  if(_inputSocket->isBound()) {
    _inputSocket->unbind();
  }
  _newMessageCond.release();
  
  Agent::finalize();
}


bool InternetAgent::isAlive() {
  return Agent::isAlive() || _senderThread->isRunning()
      || _receiverThread->isRunning() || _watchDogThread->isRunning();
}


KF_EXPORT
void init(Runtime& rt) {
  TunnelingProtocol::init(rt);
  rt.registerMessageFormat(InternetAgent::OP_ADD_CONNECTION, KType::STRING);
  rt.registerMessageFormat(InternetAgent::OP_SET_ADDRESS, KType::STRING);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new InternetAgent(rt, guid);
}
