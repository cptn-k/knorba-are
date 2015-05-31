//
//  Bubble.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 11/21/14.
//  Copyright (c) 2014 Kay Khandan. All rights reserved.
//

// KnoRBA
#include <knorba/type/KRecordType.h>

// Self
#include "Bubble.h"

// --- STATIC FIELDS --- //

SPtr<KRecordType> Bubble::TYPE;


// --- STATIC METHODS --- //

PPtr<KRecordType> Bubble::type() {
  if(TYPE.isNull()) {
    TYPE = new KRecordType("knorba.demo.bubble");
    TYPE->addMember("circle", Circle::type().AS(KType))
        ->addMember("vx", KType::REAL)
        ->addMember("vy", KType::REAL)
        ->addMember("vr", KType::REAL);
  }
  return TYPE;
}


// --- (DE)CONSTRUCTORS --- //

Bubble::Bubble()
: KRecord(type())
{
  _circle = 
}


Bubble::~Bubble() {
  // Nothing;
}


// --- METHODS --- //

