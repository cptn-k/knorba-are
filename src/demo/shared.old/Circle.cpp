//
//  Circle.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 10/17/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#include <cstdlib>

#include <kfoundation/Logger.h>

#include "Rect.h"
#include "Circle.h"

using namespace std;


k_real_t sqr(k_real_t x) {
  return x*x;
}


// --- STATIC FIELDS --- //

SPtr<KRecordType> Circle::TYPE;
SPtr<KGridType> Circle::GRID_TYPE;


// --- STATIC METHODS --- //

PPtr<KRecordType> Circle::type() {
  if(TYPE.isNull()) {
    TYPE = new KRecordType("Circle");
    TYPE->addMember("x", KType::REAL)
        ->addMember("y", KType::REAL)
        ->addMember("r", KType::REAL);
  }
  
  return TYPE;
}


PPtr<KGridType> Circle::grid_type() {
  if(GRID_TYPE.isNull()) {
    GRID_TYPE = new KGridType(type().AS(KType), 1);
  }
  return GRID_TYPE;
}


// --- CONSTRUCTORS --- //

Circle::Circle() {
  _value = type()->instantiateVal().AS(KRecord);
  set(0, 0, 0);
}


Circle::Circle(PPtr<KRecord> value)
{
  _value = value;
}


Circle::~Circle() {
  // Nothing;
}


// --- METHODS --- //

bool Circle::isInRange(k_real_t n, k_real_t a, k_real_t b) {
  return n >= a && n <= b;
}


void Circle::set(Ptr<KRecord> value) {
  if(!value->getType()->equals(type().AS(KType))) {
    throw KFException("Type mismatch");
  }
  
  _value = value;
}

void Circle::set(k_real_t x, k_real_t y, k_real_t r) {
  setX(x);
  setY(y);
  setR(r);
}


void Circle::setX(k_real_t x) {
  _value->field(0).AS(KReal)->set(x);
}


void Circle::setY(k_real_t y) {
  _value->field(1).AS(KReal)->set(y);
}


void Circle::setR(k_real_t r) {
  _value->field(2).AS(KReal)->set(r);
}


k_real_t Circle::getX() const {
  return _value->field(0).AS(KReal)->get();
}


k_real_t Circle::getY() const {
  return _value->field(1).AS(KReal)->get();
}


k_real_t Circle::getR() const {
  return _value->field(2).AS(KReal)->get();
}


bool Circle::overlapsWith(PPtr<Circle> other) const {
  k_real_t distance = sqr(getX() - other->getX())
                    + sqr(getY() - other->getY());
  
  return distance <= sqr(getR() + other->getR());
}


bool Circle::overlapsWith(PPtr<Rect> rect) const {
  k_real_t x = getX();
  k_real_t y = getY();
  
  k_real_t nearestX = 0;
  k_real_t nearestY = 0;
  
//  LOG << "Cirlce(" << x << ", " << y << ") with Rect (" << rect->getLeft() << ", "
//      << rect->getTop() << ", " << rect->getRight() << ", " << rect->getBottom()
//      << ")" << EL;
  
  if(rect->containsPoint(getX(), getY())) {
    return true;
  }
  
  if(abs(rect->getLeft() - (int)x) < abs(rect->getRight() - (int)x)) {
    nearestX = rect->getLeft();
  } else {
    nearestX = rect->getRight();
  }
  
  if(abs((int)(x - nearestX)) < getR()) {
    return true;
  }
  
  if(abs(rect->getTop() - (int)y) < abs(rect->getBottom() - (int)y)) {
    nearestY = rect->getTop();
  } else {
    nearestY = rect->getBottom();
  }
  
  if(abs((int)(y - nearestY)) < getR()) {
    return true;
  }
  
  return false;
}


PPtr<KRecord> Circle::getKValue() const {
  return _value;
}


void Circle::serialize(PPtr<ObjectSerializer> builder) const {
  _value->serialize(builder);
}
