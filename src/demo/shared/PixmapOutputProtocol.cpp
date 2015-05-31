//
//  GridOutputProtocol.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/6/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>

// KnoRBA
#include <knorba/MessageSet.h>
#include <knorba/Agent.h>
#include <knorba/type/all.h>

// Self
#include "PixmapOutputProtocol.h"

// --- STATIC FIELDS --- //

SPtr<KEnumerationType> PixmapOutputProtocol::FORMAT_T;
SPtr<KGridType> PixmapOutputProtocol::GRID_T;
SPtr<KRecordType> PixmapOutputProtocol::PUT_T;

SPtr<KString> PixmapOutputProtocol::OP_PUT
    = new KString("knorba.demo.pixmap.put");


// --- STATIC METHODS --- //


PPtr<KEnumerationType> PixmapOutputProtocol::format_t() {
  if(FORMAT_T.isNull()) {
    FORMAT_T = new KEnumerationType("knorba.demo.pixmap.format");
    
    FORMAT_T->addMember(BLACK_AND_WHITE, "BLACK_AND_WHITE")
      ->addMember(GRAY_16, "GRAY_16")
      ->addMember(GRAY_256, "GRAY_256")
      ->addMember(COLOR_16, "COLOR_16");
  }
  
  return FORMAT_T;
}

PPtr<KGridType> PixmapOutputProtocol::grid_t() {
  if(GRID_T.isNull()) {
    GRID_T = new KGridType(KType::OCTET, 2);
  }
  return GRID_T;
}


PPtr<KRecordType> PixmapOutputProtocol::put_t() {
  if(PUT_T.isNull()) {
    PUT_T = new KRecordType(OP_PUT->toUtf8String());
    PUT_T->addField("format", format_t().AS(KType))
         ->addField("phase", KType::INTEGER)
         ->addField("data", grid_t().AS(KType));
  }
  
  return PUT_T;
}


void PixmapOutputProtocol::init(Runtime& rt) {
  rt.registerMessageFormat(OP_PUT, put_t().AS(KType));
}


// --- (DE)CONSTRUCTORS --- //

PixmapOutputProtocol::PixmapOutputProtocol(Agent* owner,
    output_format_t outputFormat, PPtr<KString> serverRole)
: Protocol(owner)
{
  _serverRole = serverRole;
  _payload = new KRecord(put_t());
  _payload->setEnumeration(PUT_T_FORMAT, (k_octet_t)outputFormat);
}


void PixmapOutputProtocol::put(k_integer_t phase, PPtr<KGrid> data) {
  _payload->setInteger(PUT_T_PHASE, phase);
  _payload->setGrid(PUT_T_DATA, data);
  _agent->send(_serverRole, OP_PUT, _payload.AS(KValue));
}


void PixmapOutputProtocol::tput(k_integer_t phase, PPtr<KGrid> data) {
  _payload->setInteger(PUT_T_PHASE, phase);
  _payload->setGrid(PUT_T_DATA, data);
  _agent->tsend(_serverRole, OP_PUT, _payload.AS(KValue));
}