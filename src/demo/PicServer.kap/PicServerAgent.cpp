//
//  PicServerAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/27/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qlabel.h>


// KnoRBA
#include <knorba/Agent.h>
#include <knorba/type/all.h>

// Self
#include "PicServerAgent.h"


// --- STATIC FIELDS --- //

const SPtr<KString> PicServerAgent::OP_START = KS("knorba.demo.pic.server.start");

PicServerAgent::PicServerAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid)
{
  registerHandler((handler_t)&PicServerAgent::handleOpStart, OP_START);
}


void PicServerAgent::handleOpStart(PPtr<Message> msg) {
  int argc = 0;
  char** argv = NULL;
  QApplication app(argc, argv);
  QLabel* label = new QLabel("Hello!");
  label->show();
  app.exec();
  ALOG << "Finished" << EL;
}


//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

KF_EXPORT
void init(Runtime& rt) {
  rt.registerMessageFormat(PicServerAgent::OP_START, KType::NOTHING);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new PicServerAgent(rt, guid);
}