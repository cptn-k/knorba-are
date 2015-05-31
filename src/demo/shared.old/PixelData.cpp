//
//  PixelData.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/27/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// Self
#include "PixelData.h"

// --- STATIC FIELDS --- //

SPtr<KRecordType> PixelData::TYPE;


// --- STATIC METHODS --- //

PPtr<KRecordType> PixelData::type() {
  if(TYPE.isNull()) {
    TYPE = new KRecordType("knorba.demo.PixelData");
    TYPE->addField("location", Location2D::type().AS(KType))
        ->addField("color", RgbColorData::type().AS(KType));
  }
  return TYPE;
}


// --- (DE)CONSTRUCTORS --- //

PixelData::PixelData()
: KRecord(type())
{
  _location = new Location2D();
  bind(_location.AS(KRecord), 0);
  
  _color = new RgbColorData();
  bind(_color.AS(KRecord), 1);
}


PixelData::~PixelData() {
  // Nothing;
}


// --- METHODS --- //

PPtr<Location2D> PixelData::location() const {
  return _location;
}


PPtr<RgbColorData> PixelData::color() const {
  return _color;
}