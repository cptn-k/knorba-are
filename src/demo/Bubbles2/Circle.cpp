//
//  Circle.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 11/21/14.
//  Copyright (c) 2014 Kay Khandan. All rights reserved.
//

// KnoRBA
#include <knorba/type/KRecordType.h>
#include <knorba/type/KInteger.h>

// Self
#include "Circle.h"

// --- STATIC FIELDS --- //

SPtr<KRecordType> Circle::TYPE;


// --- STATIC METHODS --- //

PPtr<KRecordType> Circle::type() {
  if(TYPE.isNull()) {
    TYPE = new KRecordType("knorba.demo.circle");
    TYPE->addMember("x", KType::INTEGER)
        ->addMember("y", KType::INTEGER)
        ->addMember("r", KType::INTEGER);
  }
  return TYPE;
}


// --- (DE)CONSTRUCTORS --- //

Circle::Circle()
: KRecordWrapper(type())
{
  // Nothing
}


Circle::~Circle() {
  // Nothing;
}


// --- METHODS --- //

k_integer_t Circle::getX() const {
  return field(0).AS(KInteger)->get();
}


k_integer_t Circle::getY() const {
  return field(1).AS(KInteger)->get();
}


k_integer_t Circle::getR() const {
  return field(2).AS(KInteger)->get();
}


void Circle::setX(const k_integer_t x) {
  field(0).AS(KInteger)->set(x);
}


void Circle::setY(const k_integer_t y) {
  field(1).AS(KInteger)->set(y);
}


void Circle::setR(const k_integer_t r) {
  field(2).AS(KInteger)->set(r);
}
