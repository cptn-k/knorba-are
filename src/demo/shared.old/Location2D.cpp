//
//  Location2D.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/27/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#include "Location2D.h"

// --- STATIC FIELDS --- //

SPtr<KRecordType> Location2D::TYPE;


// --- STATIC METHODS --- //

PPtr<KRecordType> Location2D::type() {
  if(TYPE.isNull()) {
    TYPE = new KRecordType("knorba.demo.Location2D");
    TYPE->addField("x", KType::INTEGER)
        ->addField("y", KType::INTEGER);
  }
  return TYPE;
}


// --- (DE)CONSTRUCTORS --- //

Location2D::Location2D()
: KRecord(type())
{
  set(0, 0);
}


Location2D::Location2D(const k_integer_t x, const k_integer_t y)
: KRecord(type())
{
  set(x, y);
}


Location2D::~Location2D() {
  // Nothing;
}


// --- METHODS --- //

void Location2D::set(const k_integer_t x, const k_integer_t y) {
  setInteger(0, x);
  setInteger(1, y);
}


void Location2D::setX(const k_integer_t x) {
  setInteger(0, x);
}


void Location2D::setY(const k_integer_t y) {
  setInteger(1, y);
}


k_integer_t Location2D::getX() const {
  return getInteger(0);
}


k_integer_t Location2D::getY() const {
  return getInteger(1);
}

