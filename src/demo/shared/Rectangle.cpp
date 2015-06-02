//
//  Rectangle.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/29/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>

// KnoRBA
#include <knorba/type/all.h>

// Internal
#include "Point.h"

// Self
#include "Rectangle.h"

namespace knorba {
namespace demo {
  
// --- STATIC FIELDS --- //
  
  SPtr<KRecordType> Rectangle::TYPE;
  
  
// --- STATIC METHODS --- //
  
  SPtr<KRecordType> Rectangle::type() {
    if(TYPE.isNull()) {
      TYPE = new KRecordType("knorba.demo.Rect");
      TYPE->addField("begin", Rectangle::type().AS(KType))
      ->addField("end", Rectangle::type().AS(KType));
    }
    return TYPE;
  }
  
  
  void Rectangle::rangeToKRecord(const Range& range, PPtr<KRecord> rec) {
    Point::tupleToKRecord(range.getBegin(), rec->getRecord(BEGIN));
    Point::tupleToKRecord(range.getEnd(), rec->getRecord(END));
  }
  
  
// --- (DE)CONSTRUCTORS --- //
  
  Rectangle::Rectangle() {
    // Nothing;
  }
  
  
  Rectangle::Rectangle(const Range& other)
  : Range(other)
  {
    // Nothing;
  }
  
  
  Rectangle::Rectangle(const Tuple& begin, const Tuple& end)
  : Range(begin, end)
  {
    // Nothing;
  }
  
  
  Rectangle::Rectangle(PPtr<KRecord> r)
  : Range(Point(r->getRecord(BEGIN)), Point(r->getRecord(END)))
  {
    // Nothing;
  }
  
  
// --- METHODS --- //
  
  void Rectangle::toKRecord(PPtr<KRecord> r) const {
    rangeToKRecord(*this, r);
  }

} // demo
} // knorba