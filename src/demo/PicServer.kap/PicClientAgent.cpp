//
//  PicClientAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/29/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/BufferInputStream.h>
#include <kfoundation/PredictiveParserBase.h>
#include <kfoundation/Path.h>
#include <kfoundation/IOException.h>
#include <kfoundation/FileInputStream.h>

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/protocol/ConsoleProtocolClient.h>
#include <knorba/protocol/DisplayInfoProtocol.h>

// Internal
#include "PicProtocol.h"
#include "../shared/Rectangle.h"

// Self
#include "PicClientAgent.h"

//\/ PicClientAgent::PConsole /\///////////////////////////////////////////////

PicClientAgent::PConsole::PConsole(PicClientAgent* agent)
: ConsoleProtocolClient(agent)
{
  _iAgent = agent;
  _displayInfoReady = false;
}


void PicClientAgent::PConsole::onInputReceived(PPtr<KString> input) {
  Ptr<BufferInputStream> bis = new BufferInputStream(
      (kf_octet_t*)input->getUtf8CStr(), (kf_int32_t)input->getNOctets(),
      false);
  
  Ptr<PredictiveParserBase> parser = new PredictiveParserBase(bis.AS(InputStream));
  parser->skipSpacesAndNewLines();
  parseLine(parser);
}


void PicClientAgent::PConsole::parseLine(PPtr<PredictiveParserBase> parser) {
  string str;
  parser->readAllAlphabet(str);
  
  if(str == "help") {
    help();
    return;
  } else if(str == "quit") {
    _iAgent->send(_iAgent->_server, PicProtocol::OP_QUIT, new KTruth(T));
    return;
  } else if(str == "play") {
    play(parser);
    return;
  } else if(str == "delay") {
    delay(parser);
    return;
  }
  
  parser->skipSpaces();
  
  kf_int64_t ref = 0;
  if(parser->readNumber(ref) == 0) {
    print("Reference number expected " + parser->getCodeLocation());
    return;
  }
  
  parser->skipSpaces();
  
  if(str == "load") {
    load(parser, (k_integer_t)ref);
  } else if(str == "put") {
    put(parser, (k_integer_t)ref);
  } else if(str == "putindex") {
    putindex(parser, (k_integer_t)ref);
  } else if(str == "unput") {
    unput(parser, (k_integer_t)ref);
  } else {
    print("Unrecognized verb \"" + str + "\"");
  }
}


void PicClientAgent::PConsole::load(PPtr<PredictiveParserBase> parser,
  k_integer_t ref)
{
  parser->skipSpaces();
  
  string address;
  if(parser->readAllBeforeSpaceOrNewLine(address) == 0) {
    print("File name expeceted " + parser->getCodeLocation());
    return;
  }
  
  Ptr<Path> p = new Path(address);
  if(!p->isAbsolute()) {
    p = _iAgent->getPathToResources()->addSegement(p->getString());
  }
  
  if(!p->exists()) {
    print("File does not exist: " + p->getString());
    return;
  }
  
  Ptr<KRecord> loadCommand = new KRecord(PicProtocol::load_t());
  loadCommand->setTruth(PicProtocol::LOAD_T_RELAY, T);
  loadCommand->setInteger(PicProtocol::LOAD_T_REFERENCE, (k_integer_t)ref);
  loadCommand->getString(PicProtocol::LOAD_T_FILENAME)->set(address);
  
  try {
    loadCommand->getRaw(PicProtocol::LOAD_T_DATA)
      ->readDataFromFile(p);
  } catch(IOException& e) {
    print("Error:" + e.getMessage());
    return;
  }
  
  print("Loading File: " + p->getString());
  
  _iAgent->tsend(_iAgent->_server, PicProtocol::OP_LOAD, loadCommand.AS(KValue));
  System::sleep(500);
}


void PicClientAgent::PConsole::put(PPtr<PredictiveParserBase> parser,
  k_integer_t ref)
{
  kf_int64_t left;
  kf_int64_t top;
  kf_int64_t width;
  kf_int64_t height;
  
  if(parser->readNumber(left) == 0) {
    print("Number expected " + parser->getCodeLocation());
    return;
  }
  
  parser->skipSpaces();
  
  if(parser->readNumber(top) == 0) {
    print("Number expected " + parser->getCodeLocation());
    return;
  }
  
  parser->skipSpaces();

  if(parser->readNumber(width) == 0) {
    print("Number expected " + parser->getCodeLocation());
    return;
  }
  
  parser->skipSpaces();

  if(parser->readNumber(height) == 0) {
    print("Number expected " + parser->getCodeLocation());
    return;
  }
  
  Tuple2D begin((int)left, (int)top);
  Tuple2D size((int)width, (int)height);
  
  Rectangle r(begin, begin + size);
  
  print("Putting " + Int::toString(ref) + " at " + r);
  
  Ptr<KRecord> putCommand = new KRecord(PicProtocol::put_t());
  putCommand->setTruth(PicProtocol::PUT_T_RELAY, T);
  putCommand->setInteger(PicProtocol::PUT_T_REFERENCE, ref);
  r.toKRecord(putCommand->getRecord(PicProtocol::PUT_T_AREA));
  
  _iAgent->send(_iAgent->_server, PicProtocol::OP_PUT, putCommand.AS(KValue));
}


void PicClientAgent::PConsole::putindex(PPtr<PredictiveParserBase> parser,
    k_integer_t ref)
{
  if(!_displayInfoReady) {
    if(!_iAgent->_pDisplay.queryLocalDisplays()) {
      print("Could not query display info.");
      return;
    }
  }
  
  kf_int64_t left;
  kf_int64_t top;
  kf_int64_t width;
  kf_int64_t height;
  
  if(parser->readNumber(left) == 0) {
    print("Number expected " + parser->getCodeLocation());
    return;
  }
  
  parser->skipSpaces();
  
  if(parser->readNumber(top) == 0) {
    print("Number expected " + parser->getCodeLocation());
    return;
  }
  
  parser->skipSpaces();
  
  if(parser->readNumber(width) == 0) {
    print("Number expected " + parser->getCodeLocation());
    return;
  }
  
  parser->skipSpaces();
  
  if(parser->readNumber(height) == 0) {
    print("Number expected " + parser->getCodeLocation());
    return;
  }
  
  const Tuple& unitSize = _iAgent->_pDisplay.getLocalInfo()->getDisplayAtIndex(0).getSize();
  
  int pLeft = unitSize.at(0) * (int)left;
  int pTop = unitSize.at(1) * (int)top;
  int pRight = unitSize.at(0) * (int)(left + width);
  int pBotton = unitSize.at(1) * (int)(top + height);
  
  Rectangle r(Tuple2D(pLeft, pTop), Tuple2D(pRight, pBotton));
  
  print("Putting " + Int::toString(ref) + " at " + r);
  
  Ptr<KRecord> putCommand = new KRecord(PicProtocol::put_t());
  putCommand->setTruth(PicProtocol::PUT_T_RELAY, T);
  putCommand->setInteger(PicProtocol::PUT_T_REFERENCE, ref);
  r.toKRecord(putCommand->getRecord(PicProtocol::PUT_T_AREA));
  
  _iAgent->send(_iAgent->_server, PicProtocol::OP_PUT, putCommand.AS(KValue));
}


void PicClientAgent::PConsole::unput(PPtr<PredictiveParserBase> parser,
  k_integer_t ref)
{
  Ptr<KRecord> unputCommand = new KRecord(PicProtocol::unput_t());
  unputCommand->setTruth(PicProtocol::UNPUT_T_RELAY, T);
  unputCommand->setInteger(PicProtocol::UNPUT_T_REFERENCE, ref);
  
  print("Unputting " + Int::toString(ref));
  
  _iAgent->send(_iAgent->_server, PicProtocol::OP_UNPUT, unputCommand.AS(KValue));
}


void PicClientAgent::PConsole::delay(PPtr<PredictiveParserBase> parser) {
  parser->skipSpaces();
  
  kf_int64_t delay;
  if(parser->readNumber(delay) == 0) {
    print("Number expected " + parser->getCodeLocation());
    return;
  }
  
  print("Delay for " + Int::toString((kf_int32_t)delay) + "ms");
  
  System::sleep((kf_int32_t)delay);
}


void PicClientAgent::PConsole::play(PPtr<PredictiveParserBase> parser) {
  parser->skipSpaces();
  
  string address;
  if(parser->readAllBeforeSpaceOrNewLine(address) == 0) {
    print("Script file name expeceted " + parser->getCodeLocation());
    return;
  }

  Ptr<Path> p = new Path(address);
  if(!p->isAbsolute()) {
    p = _iAgent->getPathToResources()->addSegement(p->getString());
  }
  
  if(!p->exists()) {
    print("File does not exist: " + p->getString());
    return;
  }
  
  Ptr<FileInputStream> fis = new FileInputStream(p);
  Ptr<PredictiveParserBase> innerParser = new PredictiveParserBase(fis.AS(InputStream));
  innerParser->skipSpacesAndNewLines();
  
  print("Playing: " + p->getString());
  
  while(!innerParser->testEndOfStream()) {
    parseLine(innerParser);
    innerParser->skipSpacesAndNewLines();
  }
}


void PicClientAgent::PConsole::help() {
  print("Usage: \n"
        "    load <ref> <file-path>\n"
        "    put <ref> <left> <top> <width> <height>\n"
        "    putindex <ref> <i> <j> <cols> <rows>\n"
        "    unput <ref>\n"
        "    quit\n"
        "    help");
}


//\/ PicClientAgent /\/////////////////////////////////////////////////////////

// --- STATIC FIELDS --- //

const SPtr<KString> PicClientAgent::R_SERVER = KS("server");


// --- (DE)CONSTRUCTORS --- //

PicClientAgent::PicClientAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid),
  _pConsole(this),
  _pDisplay(this)
{
  setPassive();
  _pConsole.setServer(rt.getConsoleGuid());
  if(rt.isHead()) {
    _pConsole.subscribe();
  }
}


// --- METHODS --- //

void PicClientAgent::handlePeerConnectionRequest(PPtr<KString> role,
    const k_guid_t& guid)
{
  if(role->equals(R_SERVER)) {
    _server = guid;
    ALOG << "Server set to " << guid << EL;
  }
}


//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

KF_EXPORT
void init(Runtime& rt) {
  PicProtocol::init(rt);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new PicClientAgent(rt, guid);
}