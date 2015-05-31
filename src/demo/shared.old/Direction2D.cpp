//
//  Direction2D.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 10/17/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#include "Direction2D.h"

// --- STATIC FIELDS --- //

const Direction2D Direction2D::UP(Direction2D::NEUTRAL     , Direction2D::BACK   , 0);
const Direction2D Direction2D::UPPER_RIGHT(Direction2D::FORTH , Direction2D::BACK   , 1);
const Direction2D Direction2D::RIGHT(Direction2D::FORTH       , Direction2D::NEUTRAL, 2);
const Direction2D Direction2D::LOWER_RIGHT(Direction2D::FORTH , Direction2D::FORTH  , 3);
const Direction2D Direction2D::DOWN(Direction2D::NEUTRAL      , Direction2D::FORTH  , 4);
const Direction2D Direction2D::LOWER_LEFT(Direction2D::BACK   , Direction2D::FORTH  , 5);
const Direction2D Direction2D::LEFT(Direction2D::BACK         , Direction2D::NEUTRAL, 6);
const Direction2D Direction2D::UPPER_LEFT(Direction2D::BACK   , Direction2D::BACK   , 7);
const Direction2D Direction2D::CENTER(Direction2D::NEUTRAL    , Direction2D::NEUTRAL, 8);

const Direction2D Direction2D::ALL_DIRECTIONS[9] = {UP, UPPER_RIGHT, RIGHT, LOWER_RIGHT, DOWN, LOWER_LEFT, LEFT, UPPER_LEFT, CENTER};


// --- STATIC METHODS --- //

Direction2D Direction2D::fromOrdinal(kf_int8_t ordinal) {
  return ALL_DIRECTIONS[ordinal];
}


// --- CONSTRUCTORS --- //

Direction2D::Direction2D(direction_t x, direction_t y, kf_int8_t ordinal) {
  _ordinal = ordinal;
  _x = x;
  _y = y;
}

Direction2D::Direction2D() {
  _ordinal = 8;
  _x = NEUTRAL;
  _y = NEUTRAL;
}


// --- METHODS --- //

kf_int8_t Direction2D::getOrdinal() const {
  return _ordinal;
}


bool Direction2D::isNorth() const {
  return _y == BACK;
}

bool Direction2D::isSouth() const {
  return _y == FORTH;
}

bool Direction2D::isEast() const {
  return _x == FORTH;
}

bool Direction2D::isWest() const {
  return _x == BACK;
}

bool Direction2D::isCenter() const {
  return _x == NEUTRAL && _y == NEUTRAL;
}


void Direction2D::printToStream(ostream &os) const {
  bool space = false;
  
  if(_y == BACK) {
    os << "north";
    space = true;
  } else if(_y == FORTH) {
    os << "south";
    space = true;
  }
  
  if(_x == BACK) {
    if(space) {
      os << ' ';
    }
    os << "east";
  } else if(_x == FORTH) {
    if(space) {
      os << ' ';
    }
    os << "west";
  }
  
  if(_x == NEUTRAL && _y == NEUTRAL) {
    os << "center";
  }  
}


// --- OPERATORS --- //

bool Direction2D::operator==(const Direction2D& other) const {
  return _x == other._x && _y == other._y;
}

