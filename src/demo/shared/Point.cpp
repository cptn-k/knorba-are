//
//  Point.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/29/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>

// KnoRBA
#include <knorba/type/all.h>

// Self
#include "Point.h"

namespace knorba {
namespace demo {
  
// --- STATIC FIELDS --- //
  
  SPtr<KRecordType> Point::TYPE;
  
  
// --- STATIC METHODS --- //
  
  SPtr<KRecordType> Point::type() {
    if(TYPE.isNull()) {
      TYPE = new KRecordType("knorba.demo.Point");
      TYPE->addField("x", KType::INTEGER)
      ->addField("y", KType::INTEGER);
    }
    return TYPE;
  }
  
  
  void Point::tupleToKRecord(const Tuple& t, PPtr<KRecord> r) {
    r->setInteger(X, t.at(0));
    r->setInteger(Y, t.at(1));
  }
  
  
// --- (DE)CONSTRUCTORS --- //
  
  
  Point::Point() {
    // Nothing;
  }
  
  
  Point::Point(const Tuple& other)
  : Tuple2D(other.at(0), other.at(1))
  {
    // Nothing;
  }
  
  
  Point::Point(int x, int y)
  : Tuple2D(x, y)
  {
    // Nothing;
  }
  
  
  Point::Point(PPtr<KRecord> r)
  : Tuple2D(r->getInteger(X), r->getInteger(Y))
  {
    // Nothing;
  }
  
  
// --- METHODS --- //
  
  void Point::toKRecord(PPtr<KRecord> r) const {
    tupleToKRecord(*this, r);
  }
  
} // namespace demo
} // namespace knorba