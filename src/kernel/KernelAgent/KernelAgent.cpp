//
//  KernelAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/20/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// Std
#include <cstdlib>

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/BufferOutputStream.h>
#include <kfoundation/Thread.h>

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/Group.h>
#include <knorba/type/all.h>
#include <knorba/protocol/TunnelingProtocol.h>

// Internal
#include "../../are/KernelRuntime.h"
#include "../../are/Bundle.h"
#include "../../are/AreException.h"
#include "../../are/VirtualRuntime.h"
#include "../UnixSocketAgent/UnixSocketAgent.h"

// Self
#include "KernelAgent.h"

namespace knorba {
namespace are {
    
//\/ KernelAgent::PTunneling /\////////////////////////////////////////////////
  
  KernelAgent::PTunneling::PTunneling(KernelAgent* agent)
  : TunnelingClient(agent),
    _iAgent(agent),
    _rt((KernelRuntime&)agent->getRuntime())
  {
    // Nothing;
  }
  
  
  void KernelAgent::PTunneling::onReceive(PPtr<KRecord> msgData) {
    Ptr<Message> msg = _rt.newMessage();
    
    msg->set(msgData->getInteger(4), msgData->getLongint(3),
        msgData->getGuid(1), msgData->getAny(5)->getValue());
    
    _rt.deliver(msgData->getGuid(2), msg);
  }
  
  
  void KernelAgent::PTunneling::onReceiveBcast(PPtr<KRecord> msgData) {
    Ptr<Message> msg = _rt.newMessage();
    
    msg->set(msgData->getInteger(4), msgData->getLongint(3),
        msgData->getGuid(1), msgData->getAny(5)->getValue());
    
    _rt.deliverToAll(msg);
  }

  
  void KernelAgent::PTunneling::onAddRoute(const k_guid_t& remoteRuntime,
      const k_guid_t& remotePeer, const k_guid_t& tunnel)
  {
    PLOG << "Connected to remote runtime " << remoteRuntime << "." << EL;
    _iAgent->addPeer(R_REMOTE_KERNEL, remotePeer);
    
    if(!_iAgent->_assignment.isNull()) {
      _iAgent->doTheAssignment(remotePeer);
    } else {
      _iAgent->_pSetup.addNode(remotePeer);
    }
  }
  
  
  void KernelAgent::PTunneling::onConnectionLost(const k_guid_t& rt,
      const k_guid_t& kernel, const k_guid_t& tunnel)
  {
    PLOG << "Connection lost: " << rt << EL;
    _iAgent->onRuntimeDown(rt, kernel);
  }
  
  
//\/ KernelAgent::PromptThread /\//////////////////////////////////////////////
  
  KernelAgent::PromptThread::PromptThread(KernelAgent& owner)
  : Thread(owner.getAlias() + " console reader"),
    _owner(owner)
  {
    // Nothing
  }
  
  
  void KernelAgent::PromptThread::run() {
    cout << "This is \"" << _owner.getAlias() << "\" agent "
        << _owner.getGuid() << "." << endl;
    
    cout << "This console is retained to display status." << endl;
    cout << "Use another console to control the Kernel ARE." << endl;
    
    while (true) {
      int ch = getchar();
      if(ch == 'X') {
        _owner._rt.release();
        break;
      } else if(ch == 'Q') {
        _owner.log() << "Quitting on user command." << EL_CON;
        _owner.quit();
        break;
      } else {
        cout << "Unrecognized input '" << (char) ch << "'" << endl;
      }
    }
  }
  
  
//\/ KernelAgent::WatchdogThread /\////////////////////////////////////////////
  
  KernelAgent::WatchdogThread::WatchdogThread(KernelAgent& owner)
  : Thread(owner.getAlias() + " watchdog"),
    _owner(owner)
  {
    // Nothing;
  }
  
  
  void KernelAgent::WatchdogThread::run() {
    _owner.watchdog();
  }
  
  
//\/ KernelAgent /\////////////////////////////////////////////////////////////
  
// --- STATIC FIELDS --- //
  
  SPtr<KRecordType> KernelAgent::RUN_Q_T;
  SPtr<KRecordType> KernelAgent::RUN_A_T;
  const SPtr<KString> KernelAgent::OP_RUN_Q    = KS("knorba.kernel.run-q");
  const SPtr<KString> KernelAgent::OP_RUN_A    = KS("knorba.kernel.run-a");
  const SPtr<KString> KernelAgent::OP_SHUTDOWN = KS("knorba.kernel.shutdown");
  const SPtr<KString> KernelAgent::OP_RESTART  = KS("konrba.kernel.restart");
  const SPtr<KString> KernelAgent::OP_STATUS_Q = KS("knorba.kernel.status-q");
  const SPtr<KString> KernelAgent::OP_STATUS_A = KS("knorba.kernel.status-a");
  
  const SPtr<KString> KernelAgent::OP_AGENT_DOWN
      = KS("knorba.kernel.agent-down");
  
  const SPtr<KString> KernelAgent::OP_RUNTIME_DOWN
      = KS("knorba.kernel.runtime-down");
  
  const SPtr<KString> KernelAgent::OP_QUIT = KS("knorba.kernel.quit");
  
  const SPtr<KString> KernelAgent::R_INTERCOM  = KS("intercom");
  const SPtr<KString> KernelAgent::R_REMOTE_KERNEL = KS("remote-kernel");

  
// --- STATIC METHODS --- //
  
  PPtr<KRecordType> KernelAgent::run_q_t() {
    if(RUN_Q_T.isNull()) {
      RUN_Q_T = new KRecordType("knorba.kernel.RunQ");
      RUN_Q_T->addField("appId", KType::GUID)
             ->addField("nNodes", KType::INTEGER)
             ->addField("console", KType::GUID)
             ->addField("name", KType::STRING)
             ->addField("md5", KType::RAW)
             ->addField("bundle", KType::RAW);
      RUN_Q_T.setSelfDestruct();
    }
    return RUN_Q_T;
  }

  
  PPtr<KRecordType> KernelAgent::run_a_t() {
    if(RUN_A_T.isNull()) {
      RUN_A_T = new KRecordType("knorba.kernel.RunA");
      RUN_A_T->addField("success", KType::TRUTH)
             ->addField("bundleNeeded", KType::TRUTH)
             ->addField("appId", KType::GUID)
             ->addField("nNodes", KType::INTEGER)
             ->addField("name", KType::STRING);
      RUN_A_T.setSelfDestruct();
    }
    return RUN_A_T;
  }

  
  string KernelAgent::makeSocketPath(int pid) {
    return "/tmp/are-" + Int::toString(pid);
  }
  
  
// --- (DE)CONSTRUCTORS --- //
  
  KernelAgent::KernelAgent(KernelRuntime& rt, const k_guid_t& guid)
  : Agent(rt, guid),
    _pTunneling(this),
    _pUnixSocket(this),
    _pSetup(this),
    _pConsole(this),
    _rt(rt)
  {
    _quitFlag = false;
    _connected = false;
    _watchdogThread = new WatchdogThread(*this);
    _promptThread = new PromptThread(*this);
    _apps = new Array<AppRecord>();
    registerHandler((handler_t)&KernelAgent::handleOpStatusQ, OP_STATUS_Q);
    registerHandler((handler_t)&KernelAgent::handleOpShutdown, OP_SHUTDOWN);
    registerHandler((handler_t)&KernelAgent::handleOpRunQ, OP_RUN_Q);
    registerHandler((handler_t)&KernelAgent::handleOpRunA, OP_RUN_A);
    registerHandler((handler_t)&KernelAgent::handleOpRuntimeDown, OP_RUNTIME_DOWN);
    registerHandler((handler_t)&KernelAgent::handleOpQuit, OP_QUIT);
  }
  
  
// --- METHODS --- //
  
  void KernelAgent::watchdog() {
    for(int i = 0; i < 10 && !_quitFlag && !_connected; i++) {
      System::sleep(100);
    }
    
    if(!_quitFlag && !_connected) {
      ALOG_ERR << "Time out expired while trying to perform \"" << *_assignment
          << "\"." << EL;
      quit();
    }
  }
  
  
  void KernelAgent::doTheAssignment(const k_guid_t& remotePeer) {
    LOG << "Performing assignment: " << *_assignment << EL;
    
    _connected = true;
    _localKernel = remotePeer;
    
    if(_assignment->equals(OP_STATUS_Q)) {
      
      doKernelPrintStatusAssignment(remotePeer);
      
    } else if(_assignment->equals(OP_SHUTDOWN)) {
      
      send(remotePeer, OP_SHUTDOWN, new KTruth(T));
      ALOG << "Shutdown command sent." << EL_CON;
      quit();
      
    } else if(_assignment->equals(OP_RUN_Q)) {
      
      doKernelRunAssignment(remotePeer);
      
    }
    
    ALOG << "End assignment." << EL;
  }
  
  
  void KernelAgent::doKernelPrintStatusAssignment(const k_guid_t& remotePeer) {
    Ptr<Message> response = tsend(remotePeer, OP_STATUS_Q, KValue::NOTHING, 1000);
    
    if(response.isNull()) {
      ALOG_ERR << "Status query failed. No response from the Kernel." << EL;
    } else {
      ALOG << *response->getPayload().AS(KString) << EL_CON;
    }
    
    quit();
  }
  
  
  void KernelAgent::doKernelRunAssignment(const k_guid_t& remotePeer) {
    _pConsole.start();
    ALOG << "Sending run command: " << *_assignmentArg << EL;
    send(remotePeer, OP_RUN_Q, _assignmentArg);
  }
  
  
  void KernelAgent::runOnCluster(PPtr<Bundle> bundle, const k_guid_t& kernel,
      const k_guid_t& console)
  {
    k_guid_t guid = KGlobalUid::zero();
    KGlobalUid::randomizeAppId(guid);
    
    AppRecord& r = _apps->push();
    r.shellKernel = kernel;
    r.console = console;
    r.appId = guid;
    r.refCount = _pSetup.getCluster()->getCount() + 1;
    
    _pConsole.addRelayPath(guid, console);
    
    string text = "Running a new instance of application \""
        + bundle->getAppName() + "\" with ID "
        + KGlobalUid::appIdToString(guid) + " on " + Int::toString(r.refCount)
        + " node(s).";
    
    ALOG << text << EL;
    
    ConsoleProtocolClient pShellConsole(this);
    pShellConsole.setServer(console);
    pShellConsole.print(text);
    
    runLocally(bundle, guid, _pConsole.getServer(), r.refCount);

    if(!_pSetup.getCluster()->isEmpty()) {
      Ptr<KRecord> command = new KRecord(run_q_t());
      command->getString(RUN_Q_NAME)->set(bundle->getAppName());
      command->setGuid(RUN_Q_APPID, guid);
      command->getRaw(RUN_Q_MD5)->set(bundle->getMd5(), Bundle::SIZE_OF_MD5_IN_OCTETS);
      command->setGuid(RUN_Q_CONSOLE, _pConsole.getServer());
      command->setInteger(RUN_Q_N_NODES, r.refCount);
      send(_pSetup.getCluster(), OP_RUN_Q, command.AS(KValue));
    }
  }
  
  
  void KernelAgent::runLocally(PPtr<Bundle> bundle, k_guid_t guid,
      const k_guid_t& console, k_integer_t nNodes)
  {
    _pSetup.fixRank();
    
    k_integer_t rank = _pSetup.getRank();
    
    for(int i = _pSetup.getNumberOfNodes() - 1; i >= 0; i--) {
      guid.nodeRank = _pSetup.getRankForKernelAtIndex(i);
      
      if(guid.nodeRank == rank) {
        continue;
      }
      
      _pTunneling.addVtrRoute(guid, _pSetup.getGuidForKernelAtIndex(i));
    }

    guid.nodeRank = rank;
    
    PPtr<VirtualRuntime> vrt = _rt.newVirtualRuntime(guid);
    vrt->setConsole(console);
    vrt->setNodeCount(nNodes);
    
//    try {
//      vrt->runBundle(bundle);
//    } catch (KFException& e) {
//      ALOG_ERR << e.getMessage() << EL;
//      ALOG << e << EL;
//    }
    
    _rt.runBundleInMainThread(vrt, bundle);
  }
  
  
  int KernelAgent::getIndexForApp(const k_guid_t& app) const {
    for(int i = _apps->getSize() - 1; i >= 0; i--) {
      if(KGlobalUid::areOnTheSameApp(app, _apps->at(i).appId)) {
        return i;
      }
    }
    return -1;
  }
  
  
  void KernelAgent::onRuntimeDown(const k_guid_t& rt, const k_guid_t& kernel) {
    if(KGlobalUid::areOnTheSameApp(rt, kernel)) {
      if(kernel == _localKernel) {
        ALOG << "Connection lost to local kernel: " << kernel << EL;
        _pConsole.print("Connection to local kenrel lost.");
        _pConsole.stop();
        return;
      }
      
      _pSetup.removeNode(kernel);
    }
    
    _rt.handleRuntimeDisconnect(rt);
    
    int index = getIndexForApp(rt);
    if(index == -1) {
      return;
    }
    
    ALOG << "VRT Down: " << rt << EL;
    
    AppRecord& r = _apps->at(index);
    r.refCount--;
    if(r.refCount <= 0) {
      string txt = "Execution of application instance completed: "
          + KGlobalUid::appIdToString(rt);
      
      ALOG << txt << EL;
      
      ConsoleProtocolClient pconsole(this);
      pconsole.setServer(r.console);
      pconsole.print(txt);
      
      _pConsole.removeRelayPath(rt);
      send(r.shellKernel, OP_QUIT, KValue::NOTHING);
      
      _apps->remove(index);
    }
  }
  
  
  void KernelAgent::proxySend(const k_guid_t &target, PPtr<Message> msg) {
    if(_quitFlag) {
      return;
    }
    
    _pTunneling.send(target, msg);
  }
  
  
  void KernelAgent::proxySendToAll(PPtr<Message> msg) {
    if(_quitFlag) {
      return;
    }
    
    _pTunneling.sendToAll(msg);
  }
  
  
  void KernelAgent::kernelRun() {
    _promptThread->start();
  }

  
  void KernelAgent::kernelPrintStatus(int pid) {
    _assignment = OP_STATUS_Q;
    _pUnixSocket.addConnections(makeSocketPath(pid));
    _watchdogThread->start();
  }
  
  
  void KernelAgent::kernelShutdown(int pid) {
    _assignment = OP_SHUTDOWN;
    _pUnixSocket.addConnections(makeSocketPath(pid));
    _watchdogThread->start();
  }
  
  
  void KernelAgent::kernelRunBundle(int pid, const string& bundlePath) {
    Ptr<Bundle> b = new Bundle(new Path(bundlePath), _rt.getHome());
    b->pack();
    _assignment = OP_RUN_Q;
    Ptr<KRecord> arg = new KRecord(run_q_t());
    arg->getString(RUN_Q_NAME)->set(b->getAppName());
    arg->setGuid(RUN_Q_APPID, KGlobalUid::zero());
    arg->getRaw(RUN_Q_MD5)->set(b->getMd5(), Bundle::SIZE_OF_MD5_IN_OCTETS);
    arg->setGuid(RUN_Q_CONSOLE, _pConsole.getServer());
    arg->setInteger(RUN_Q_N_NODES, -1);
    _assignmentArg = arg.AS(KValue);
    _pUnixSocket.addConnections(makeSocketPath(pid));
    _watchdogThread->start();
  }
  
  
  void KernelAgent::anounceDeadAgent(const k_guid_t& guid) {
    sendToAll(OP_AGENT_DOWN, new KGlobalUid(guid));
  }
  
  
  void KernelAgent::anounceDeadRuntime(const k_guid_t& guid) {
    send(R_REMOTE_KERNEL, OP_RUNTIME_DOWN, new KGlobalUid(guid));
    onRuntimeDown(guid, getGuid());
  }
  
  
  bool KernelAgent::isHead() const {
    return _pSetup.isHead();
  }
  
  
  const k_guid_t& KernelAgent::getConsoleGuid() const {
    return _pConsole.getServer();
  }
  
  
  // Message Handlers //
  
  void KernelAgent::handleOpShutdown(PPtr<Message> msg) {
    ALOG << "Shutdown command received from " << msg->getSender() << "."
        << EL_CON;
    
    if(msg->getPayload().AS(KTruth)->get() == T) {
      send(_pSetup.getCluster(), OP_SHUTDOWN, new KTruth(F));
      ALOG << "Sent shutdown command to the cluster." << EL;
      System::sleep(1000);
    }
    
    _rt.release();
  }
  
  
  void KernelAgent::handleOpRestart(PPtr<Message> msg) {
    LOG_ERR << "handleOpRestart()" << EL;
  }

  
  void KernelAgent::handleOpStatusQ(PPtr<Message> msg) {
    respond(msg, OP_STATUS_A, new KString(_rt.toString()));
  }
  
  
  void KernelAgent::handleOpRunQ(PPtr<Message> msg) {
    PPtr<KRecord> q = msg->getPayload().AS(KRecord);
    PPtr<KString> appName = q->getString("name");
    
    LOG << "Received run command: " << *q << EL;

    PPtr<KRecord> a = new KRecord(run_a_t());
    a->setGuid(RUN_A_APPID, q->getGuid(RUN_Q_APPID));
    a->getString(RUN_A_NAME)->set(appName.AS(KValue));
    a->setInteger(RUN_A_N_NODES, q->getInteger(RUN_Q_N_NODES));
    
    Ptr<Path> karPath = _rt.getHome()->addSegement("repo")
        ->addSegement(appName->toUtf8String());
    
    bool doRun = false;
    
    if(q->getRaw(RUN_Q_BUNDLE)->getNOctets() > 0) {
      Ptr<Path> downloadPath = _rt.getHome()->addSegement("data/download");
      if(!downloadPath->exists()) {
        downloadPath->makeDir();
      }
      
      karPath = downloadPath->addSegement(
          q->getString(RUN_Q_NAME)->toUtf8String() + ".kar");
      
      if(karPath->exists()) {
        karPath->remove();
      }
      
      q->getRaw(RUN_Q_BUNDLE)->writeDataToFile(karPath);
      ALOG << "Bundle saved to file: " << *karPath << EL;
    }
    
    try {
      Ptr<Bundle> b = new Bundle(karPath, _rt.getHome());
      b->pack();
      
      if(b->checkMd5(q->getRaw(RUN_Q_MD5)->getData())) {
        a->setTruth(RUN_A_SUCESS, T);
        a->setTruth(RUN_A_BUNDLE_NEEDED, F);
        doRun = true;
      } else {
        ALOG << "Bundle \"" << b->getAppName() << "\" is outdated. "
          "Requesting for update." << EL;
        a->setTruth(RUN_A_SUCESS, F);
        a->setTruth(RUN_A_BUNDLE_NEEDED, T);
      }
      if(doRun) {
        if(q->getGuid(RUN_Q_APPID) == KGlobalUid::zero()) {
          runOnCluster(b, msg->getSender(), q->getGuid(RUN_Q_CONSOLE));
        } else {
          runLocally(b, q->getGuid(RUN_Q_APPID), q->getGuid(RUN_Q_CONSOLE),
              q->getInteger(RUN_Q_N_NODES));
        }
      }
    } catch(AreException& e) {
      ALOG << e.getMessage() << EL;
      ALOG << "Requesting for the updated bundle." << EL;
      a->setTruth(RUN_A_SUCESS, F);
      a->setTruth(RUN_A_BUNDLE_NEEDED, T);
    }
    
    respond(msg, OP_RUN_A, a.AS(KValue));
  }
  
  
  void KernelAgent::handleOpRunA(PPtr<Message> msg) {
    PPtr<KRecord> a = msg->getPayload().AS(KRecord);
    
    if(a->getTruth(RUN_A_BUNDLE_NEEDED) == T) {
      Ptr<Path> karPath = _rt.getHome()->addSegement("repo")
          ->addSegement(a->getString(RUN_A_NAME)->toUtf8String() + ".kar");
      
      Ptr<Bundle> b = new Bundle(karPath, _rt.getHome());
      b->pack();
      
      Ptr<KRecord> q = new KRecord(run_q_t());
      q->getString(RUN_Q_NAME)->set(b->getAppName());
      q->setGuid(RUN_Q_APPID, a->getGuid(RUN_A_APPID));
      q->getRaw(RUN_Q_MD5)->set(b->getMd5(), Bundle::SIZE_OF_MD5_IN_OCTETS);
      q->getRaw(RUN_Q_BUNDLE)->readDataFromFile(b->getKarPath());
      q->setGuid(RUN_Q_CONSOLE, _pConsole.getServer());
      q->setInteger(RUN_Q_N_NODES, a->getInteger(RUN_A_N_NODES));
      
      ALOG << "Sending updated bundle \"" << b->getAppName() << "\" to "
        << msg->getSender() << EL;
      
      respond(msg, OP_RUN_Q, q.AS(KValue));
      
      return;
    }

    if(a->getTruth(RUN_A_SUCESS) == T) {
      if(!_assignment.isNull()) {
        ALOG << "Run command sent successfully." << EL;
      }
    } else {
      ALOG_ERR << "Error executing run command by " << msg->getSender() << EL;
      quit();
    }
  }
  
  
  void KernelAgent::handleOpRuntimeDown(PPtr<Message> msg) {
    const k_guid_t& guid = msg->getPayload().AS(KGlobalUid)->get();
    _pTunneling.removeRoute(guid);
    onRuntimeDown(guid, msg->getSender());
  }
  
  
  void KernelAgent::handleOpQuit(PPtr<Message> msg) {
    if(!_assignment.isNull()) {
      ALOG << "Quit command received from local kernel." << EL;
      _pConsole.stop();
    }
  }
  
  
  void KernelAgent::handlePeerConnectionRequest(PPtr<KString> role,
      const k_guid_t& guid)
  {
    if(role->equals(R_INTERCOM)) {
      _pTunneling.addTunnel(guid);
      _pUnixSocket.setServer(guid);
      _pUnixSocket.setAddress(makeSocketPath(System::getPid()));
    } else if(role->equals(TunnelingProtocol::R_TUNNEL)) {
      _pTunneling.addTunnel(guid);
    } else {
      Agent::handlePeerConnectionRequest(role, guid);
    }
  }
  
  void KernelAgent::handlePeerDisconnected(PPtr<KString> role, const k_guid_t& guid)
  {
    if(guid == _pConsole.getServer()) {
      quit();
    }
  }

  
  void KernelAgent::finalize() {
    _quitFlag = true;
    Agent::finalize();
  }
  

//\/ KernelAgentLoader /\//////////////////////////////////////////////////////
  
  KernelAgentLoader::KernelAgentLoader()
  : AgentLoader("KernelAgent", NULL)
  {
    // Nothing;
  }
  
  
  void KernelAgentLoader::init(Runtime& rt) {
    rt.registerMessageFormat(Agent::OP_ACK, KType::NOTHING);
    rt.registerMessageFormat(Agent::OP_NG, KType::STRING);
    rt.registerMessageFormat(KernelAgent::OP_RUN_Q, KernelAgent::run_q_t().AS(KType));
    rt.registerMessageFormat(KernelAgent::OP_RUN_A, KernelAgent::run_a_t().AS(KType));
    rt.registerMessageFormat(KernelAgent::OP_SHUTDOWN, KType::TRUTH);
    rt.registerMessageFormat(KernelAgent::OP_RESTART, KType::NOTHING);
    rt.registerMessageFormat(KernelAgent::OP_STATUS_Q, KType::NOTHING);
    rt.registerMessageFormat(KernelAgent::OP_STATUS_A, KType::STRING);
    rt.registerMessageFormat(KernelAgent::OP_AGENT_DOWN, KType::GUID);
    rt.registerMessageFormat(KernelAgent::OP_RUNTIME_DOWN, KType::GUID);
    TunnelingClient::init(rt);
    ConsoleProtocolClient::init(rt);
    ClusterSetupProtocol::init(rt);
  }
  
  
  Agent* KernelAgentLoader::instantiate(Runtime& rt, const k_guid_t& guid) {
    return new KernelAgent((KernelRuntime&)rt, guid);
  }

  
} // are
} // knorba
