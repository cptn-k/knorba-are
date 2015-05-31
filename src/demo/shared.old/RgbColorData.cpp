//
//  RGBColorData.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/24/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#include "RgbColorData.h"

// --- STATIC FIELDS --- //

SPtr<KRecordType> RgbColorData::TYPE;


// --- STATIC METHODS --- //

PPtr<KRecordType> RgbColorData::type() {
  if(TYPE.isNull()) {
    TYPE = new KRecordType("knorba.demo.color");
    TYPE->addField("r", KType::OCTET)
        ->addField("g", KType::OCTET)
        ->addField("b", KType::OCTET);
  }
  return TYPE;
}


// --- (DE)CONSTRUCTORS --- //

RgbColorData::RgbColorData()
: KRecord(type())
{
  set(0, 0, 0);
}


RgbColorData::RgbColorData(k_octet_t r, k_octet_t g, k_octet_t b)
: KRecord(type())
{
  set(r, g, b);
}


RgbColorData::~RgbColorData() {
  // Nothing;
}


// --- METHODS --- //

void RgbColorData::set(k_octet_t r, k_octet_t g, k_octet_t b) {
  setR(r);
  setG(g);
  setB(b);
}


void RgbColorData::setR(k_octet_t r) {
  setOctet(0, r);
}


void RgbColorData::setG(k_octet_t g) {
  setOctet(1, g);
}


void RgbColorData::setB(k_octet_t b) {
  setOctet(2, b);
}


k_octet_t RgbColorData::getR() const {
  return getOctet(0);
}


k_octet_t RgbColorData::getG() const {
  return getOctet(1);
}


k_octet_t RgbColorData::getB() const {
  return getOctet(2);
}
