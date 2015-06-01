//
//  PicProtocol.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/29/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/type/all.h>

// Internal
#include "Point.h"
#include "Rectangle.h"

// Self
#include "PicProtocol.h"

using namespace demo;

// --- STATIC FIELDS --- //

SPtr<KRecordType> PicProtocol::LOAD_T;
SPtr<KRecordType> PicProtocol::PUT_T;

const SPtr<KString> PicProtocol::OP_LOAD = KS("knorba.demo.pic.load");
const SPtr<KString> PicProtocol::OP_PUT = KS("knorba.demo.pic.put");
const SPtr<KString> PicProtocol::OP_UNPUT = KS("konrba.demo.pic.unput");
const SPtr<KString> PicProtocol::OP_CLEAR = KS("knorba.demo.pic.clear");


// --- STATIC METHODS --- //

SPtr<KRecordType> PicProtocol::load_t() {
  if(LOAD_T.isNull()) {
    LOAD_T = new KRecordType("knorba.demo.pic.Load");
    LOAD_T->addField("reference", KType::INTEGER)
          ->addField("fileName", KType::STRING)
          ->addField("data", KType::RAW);
  }
  return LOAD_T;
}


SPtr<KRecordType> PicProtocol::put_t() {
  if(PUT_T.isNull()) {
    PUT_T = new KRecordType("knorba.demo.pic.Put");
    PUT_T->addField("reference", KType::INTEGER)
         ->addField("area", Rectangle::type().AS(KType));
  }
  return PUT_T;
}


void PicProtocol::init(Runtime& r) {
  r.registerMessageFormat(OP_LOAD, load_t().AS(KType));
  r.registerMessageFormat(OP_PUT, put_t().AS(KType));
  r.registerMessageFormat(OP_UNPUT, KType::INTEGER);
  r.registerMessageFormat(OP_CLEAR, KType::NOTHING);
}