//
//  Rect.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 10/17/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#include <knorba/type/KRecordType.h>
#include "Rect.h"


// --- STATIC FIELDS --- //

SPtr<KRecordType> Rect::TYPE;


// --- STATIC METHODS --- //

Ptr<KRecordType> Rect::type() {
  if(TYPE.isNull()) {
    TYPE = new KRecordType("Rect");
    TYPE->addField("x1", KType::INTEGER);
    TYPE->addField("y1", KType::INTEGER);
    TYPE->addField("x2", KType::INTEGER);
    TYPE->addField("y2", KType::INTEGER);
  }
  return TYPE;
}


// --- CONSTRUCTORS --- //

Rect::Rect()
: KRecord(type())
{
  set(Range(Tuple2D::ZERO, Tuple2D::ZERO));
}


Rect::Rect(const Range& v)
: KRecord(type())
{
  set(v);
}


// --- METHODS --- //

void Rect::set(const Range& v) {
  setInteger(0, v.getBegin().at(0));
  setInteger(1, v.getBegin().at(1));
  setInteger(2, v.getEnd().at(0));
  setInteger(3, v.getEnd().at(1));
}


Range Rect::get() const {
  return Range(Tuple2D(getInteger(0), getInteger(1)),
               Tuple2D(getInteger(2), getInteger(3)));
}
