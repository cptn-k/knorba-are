//
//  ConsoleAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/3/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// Std
#include <cstdlib>

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/ManagedArray.h>
#include <kfoundation/Condition.h>
#include <kfoundation/Logger.h>
#include <kfoundation/Path.h>
#include <kfoundation/Thread.h>

// KnoRBA
#include <knorba/Group.h>
#include "/usr/local/include/knorba/protocol/ConsoleProtocolClient.h"

// NCurses
#include <curses.h>

// Self
#include "ConsoleAgent.h"

//\/ ConsoleAgent::RelayPath /\////////////////////////////////////////////////

ConsoleAgent::RelayPath::RelayPath(const k_guid_t& vrt, const k_guid_t& console)
: sourceRuntime(vrt),
  targetConsole(console)
{
  subscribers = new Group();
}


//\/ ConsoleAgent::PromptThread /\/////////////////////////////////////////////

ConsoleAgent::PromptThread::PromptThread(ConsoleAgent& owner)
: Thread(owner.getAlias() + " prompt"),
  _owner(owner)
{
  // Nothing;
}


void ConsoleAgent::PromptThread::run() {
  _owner.promptLoop();
}


//\/ ConsoleAgent /\///////////////////////////////////////////////////////////

// --- STATIC FIELDS --- //

const SPtr<KString> ConsoleAgent::R_SUBSCRIBER = KS("subscriber");


// --- (DE)CONSTRUCTORS --- //

ConsoleAgent::ConsoleAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid)
{
  setPassive();
  
  _stopFlag = false;
  _isReading = false;
  _promptThread = new PromptThread(*this);
  _relayPaths = new ManagedArray<RelayPath>();
  
  _titleBar = NULL;
  _outputWindow = NULL;
  _inputWindow = NULL;
  
  _logger.addChannel("file", getPathToData()->getString() + "/output.log");
  
  _logger.log(Logger::L3) << EL;
  _logger.log(Logger::L3) << " * * * " << EL;
  _logger.log(Logger::L3) << EL;

  registerHandler((handler_t)&ConsoleAgent::handleOpStart,
      ConsoleProtocolClient::OP_START);
  
  registerHandler((handler_t)&ConsoleAgent::handleOpStop,
      ConsoleProtocolClient::OP_STOP);
  
  registerHandler((handler_t)&ConsoleAgent::handleOpPrint,
      ConsoleProtocolClient::OP_PRINT);
  
  registerHandler((handler_t)&ConsoleAgent::handleOpInput,
      ConsoleProtocolClient::OP_INPUT);
  
  registerHandler((handler_t)&ConsoleAgent::handleOpSubscribe,
      ConsoleProtocolClient::OP_SUBSCRIBE);
  
  registerHandler((handler_t)&ConsoleAgent::handleOpUnsubscribe,
      ConsoleProtocolClient::OP_UNSUBSCRIBE);
  
  registerHandler((handler_t)&ConsoleAgent::handleOpAddRelayPath,
      ConsoleProtocolClient::OP_ADD_RELAY_PATH);
  
  registerHandler((handler_t)&ConsoleAgent::handleOpRemoveRelayPath,
      ConsoleProtocolClient::OP_REMOVE_RELAY_PATH);
}


// --- METHODS --- //

void ConsoleAgent::promptLoop() {
  ALOG << "Prompt loop started." << EL;
  
  // ncurses
  initscr();
  halfdelay(3);
  noecho();
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  
  int nTerminalRows, nTerminalCols;
  getmaxyx(stdscr, nTerminalRows, nTerminalCols);
  
  _titleBar = newwin(1, nTerminalCols, 0, 0);
  wattron(_titleBar, COLOR_PAIR(1));
  wattron(_titleBar, A_BOLD);
  wprintw(_titleBar, " Welcome to KnoRBA ConsoleAgent's Terminal");
  whline(_titleBar, ' ', nTerminalCols);
  wrefresh(_titleBar);
  
  _outputWindow = newwin(nTerminalRows - 5, nTerminalCols - 2, 2, 1);
  scrollok(_outputWindow, true);
  
  _inputWindow = newwin(3, nTerminalCols - 2, nTerminalRows - 3, 1);
  printInput();
  
  int ch = 0;
  bool _quitOnCommand = false;
  
  while(!_stopFlag) {
    ch = wgetch(_inputWindow);
    
    if(ch == ERR) {
      continue;
    } else if(ch == '\n') {
      if(_input == "\\close") {
        _logger.log(Logger::L3) << "Terminal closed on user command." << EL;
        _quitOnCommand = true;
        quit();
      } else {
        wattron(_inputWindow, COLOR_PAIR(2));
        wprintw(_outputWindow, "> %s\n", _input.c_str());
        wattroff(_inputWindow, COLOR_PAIR(2));
        wrefresh(_outputWindow);
        _logger.log(Logger::L3) << "> " << _input << EL;
        sendInput();
        _input.clear();
        printInput();
      }
    } else if(ch == 127 || ch == 8) {
      _input.erase(_input.size() - 1, 1);
      printInput();
    } else {
      _input.push_back(ch);
      printInput();
    }
  }
  
  if(!_quitOnCommand) {
    wmove(_inputWindow, 1, 0);
    whline(_inputWindow, ' ', nTerminalCols);
    wattron(_inputWindow, A_BOLD);
    wprintw(_inputWindow, "Program terminated. Press any key to close this terminal ");
    wattron(_inputWindow, A_BLINK);
    wprintw(_inputWindow, "> > > ");
    wrefresh(_inputWindow);
    getchar();
  }
  
  
  delwin(_titleBar);
  _titleBar = NULL;
  
  delwin(_outputWindow);
  _outputWindow = NULL;
  
  delwin(_inputWindow);
  _inputWindow = NULL;
  
  nocbreak();
  endwin();
  
  ALOG << "Prompt loop ended." << EL;
}


void ConsoleAgent::printInput() {
  if(IS_NULL(_inputWindow)) {
    return;
  }
  
  int nRows, nCols;
  getmaxyx(_inputWindow, nRows, nCols);
  
  wclear(_inputWindow);
  whline(_inputWindow, '-', nCols);
  
  wmove(_inputWindow, 1, 0);
  wattron(_inputWindow, COLOR_PAIR(2));
  wprintw(_inputWindow, "> ");
  wattroff(_inputWindow, COLOR_PAIR(2));
  wprintw(_inputWindow, _input.c_str());
  wrefresh(_inputWindow);
}


int ConsoleAgent::getIndexForApp(const k_guid_t& appId) {
  for(int i = _relayPaths->getSize() - 1; i >= 0; i--) {
    if(KGlobalUid::areOnTheSameApp(_relayPaths->at(i)->sourceRuntime, appId)) {
      return i;
    }
  }
  return -1;
}


int ConsoleAgent::getIndexForConsole(const k_guid_t& console) {
  for(int i = _relayPaths->getSize() - 1; i >= 0; i--) {
    if(_relayPaths->at(i)->targetConsole == console) {
      return i;
    }
  }
  return -1;
}


void ConsoleAgent::print(const char* str) {
  if(NOT_NULL(_outputWindow)) {
    wprintw(_outputWindow, "%s\n", str);
    wrefresh(_outputWindow);
    printInput();
  } else {
    printf("%s\n", str);
  }
}


void ConsoleAgent::sendInput() {
  send(R_SUBSCRIBER, ConsoleProtocolClient::OP_INPUT, new KString(_input));
}


void ConsoleAgent::unsubscribe(const k_guid_t& guid) {
  removePeer(R_SUBSCRIBER, guid);
}


// Handlers //

void ConsoleAgent::handleOpStart(PPtr<Message> msg) {
  if(!_promptThread->isRunning()) {
    _promptThread->start();
  }
}


void ConsoleAgent::handleOpStop(PPtr<Message> msg) {
  quit();
}


void ConsoleAgent::handleOpPrint(PPtr<Message> msg) {
  int index = getIndexForApp(msg->getSender());
  if(index >= 0) {
    System::sleep(rand()%50);
    send(_relayPaths->at(index)->targetConsole,
         ConsoleProtocolClient::OP_PRINT, msg->getPayload());
  } else {
    _logger.log(Logger::L3) << *msg->getPayload() << EL;
    print(msg->getPayload().AS(KString)->getUtf8CStr());
  }
}


void ConsoleAgent::handleOpInput(PPtr<Message> msg) {
  int index = getIndexForConsole(msg->getSender());
  if(index < 0) {
    ALOG << "No relay path. Ignoring input: " << *msg->getPayload() << EL;
  } else {
    send(_relayPaths->at(index)->subscribers, ConsoleProtocolClient::OP_INPUT,
        msg->getPayload());
  }
}


void ConsoleAgent::handleOpSubscribe(PPtr<Message> msg) {
  const k_guid_t& sender = msg->getSender();
  int index = getIndexForApp(sender);
  if(index < 0) {
    addPeer(R_SUBSCRIBER, sender);
  } else {
    _relayPaths->at(index)->subscribers->add(sender);
  }
}


void ConsoleAgent::handleOpUnsubscribe(PPtr<Message> msg) {
  unsubscribe(msg->getSender());
}


void ConsoleAgent::handleOpAddRelayPath(PPtr<Message> msg) {
  PPtr<KRecord> rec = msg->getPayload().AS(KRecord);
  const k_guid_t& vrt = rec->getGuid(0);
  const k_guid_t& con = rec->getGuid(1);
  
  if(getIndexForApp(vrt) >= 0) {
    ALOG_WRN << "Duplicate add request for relay path to app " << vrt << EL;
  } else {
    _relayPaths->push(new RelayPath(vrt, con));
    send(con, ConsoleProtocolClient::OP_SUBSCRIBE, KValue::NOTHING);
    ALOG << "Added relay path: " << vrt << " --> " << con << EL;
  }
}


void ConsoleAgent::handleOpRemoveRelayPath(PPtr<Message> msg) {
  const k_guid_t& vrt = msg->getPayload().AS(KGlobalUid)->get();
  int index = getIndexForApp(vrt);
  if(index < 0) {
    ALOG_WRN << "Ignoring removal request. No path is registered to " << vrt
        << EL;
  } else {
    PPtr<RelayPath> p = _relayPaths->at(index);
    
    ALOG << "Removing relay path: " << p->sourceRuntime << " --> "
        << p->targetConsole << EL;
    
    send(p->targetConsole, ConsoleProtocolClient::OP_UNSUBSCRIBE,
         KValue::NOTHING);
    
    _relayPaths->remove(index);
  }
}


void ConsoleAgent::finalize() {
  _stopFlag = true;
  Agent::finalize();
}


bool ConsoleAgent::isAlive() {
  return Agent::isAlive() || _promptThread->isRunning();
}


void ConsoleAgent::handlePeerDisconnected(PPtr<KString> role, const k_guid_t& guid)
{
  if(role->equals(R_SUBSCRIBER)) {
    unsubscribe(guid);
  }
}


// --- GLOBAL --- //

KF_EXPORT
void init(Runtime& rt) {
  ConsoleProtocolClient::init(rt);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new ConsoleAgent(rt, guid);
}
