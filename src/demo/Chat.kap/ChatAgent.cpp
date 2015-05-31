//
//  ChatAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/20/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/type/all.h>

// Protocol
#include <knorba/protocol/ConsoleProtocolClient.h>

// Self
#include "ChatAgent.h"


//\/ ChatAgent::PConsole /\////////////////////////////////////////////////////

ChatAgent::PConsole::PConsole(Agent* agent)
: ConsoleProtocolClient(agent)
{
  // Nothing;
}


void ChatAgent::PConsole::onInputReceived(PPtr<KString> input) {
  dynamic_cast<ChatAgent*>(_agent)->processInput(input);
}


//\/ ChatAgent /\//////////////////////////////////////////////////////////////

// --- STATIC FIELDS --- //

SPtr<KRecordType> ChatAgent::MESSAGE_T;
const SPtr<KString> ChatAgent::OP_MESSAGE = KS("knorba.demo.chat.message");


// --- STATIC METHODS --- //

SPtr<KRecordType> ChatAgent::message_t() {
  if(MESSAGE_T.isNull()) {
    MESSAGE_T = new KRecordType("knorba.demo.chat.Message");
    MESSAGE_T->addField("sender", KType::STRING)
      ->addField("content", KType::STRING);
  }
  return MESSAGE_T;
}


// --- (DE)CONSTRUCTORS --- //

ChatAgent::ChatAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid),
  _pConsole(this)
{
  registerHandler((handler_t)&ChatAgent::handleOpMessage, OP_MESSAGE);
  
  _pConsole.setServer(getRuntime().getConsoleGuid());
  _pConsole.subscribe();
  _pConsole.print("Enter a nickname...");
}


// --- METHODS --- //

void ChatAgent::processInput(PPtr<KString> input) {
  if(_nickName.empty()) {
    _nickName = input->toUtf8String();
    _pConsole.print("You shall be known as \"" + _nickName + "\".");
    _pConsole.print("Type anything to send. Enter \"quit\" to finish.");
  } else if(input->equals("quit")) {
    _pConsole.print("Bye!");
    quit();
  } else {
    Ptr<KRecord> message = new KRecord(message_t());
    message->getString(MESSAGE_T_SENDER)->set(_nickName);
    message->getString(MESSAGE_T_CONTENT)->set(input.AS(KValue));
    sendToAll(OP_MESSAGE, message.AS(KValue));
  }
}


void ChatAgent::handleOpMessage(PPtr<Message> msg) {
  PPtr<KRecord> rec = msg->getPayload().AS(KRecord);
  
  _pConsole.print(rec->getString(MESSAGE_T_SENDER)->toUtf8String()
      + " says: " + rec->getString(MESSAGE_T_CONTENT)->toUtf8String());
}


//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

KF_EXPORT
void init(Runtime& rt) {
  rt.registerMessageFormat(ChatAgent::OP_MESSAGE, ChatAgent::message_t().AS(KType));
  ConsoleProtocolClient::init(rt);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new ChatAgent(rt, guid);
}