//
//  Direction2D.h
//  ARE
//
//  Created by Hamed KHANDAN on 10/17/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef __ARE__Direction2D__
#define __ARE__Direction2D__

#include <kfoundation/definitions.h>
#include <kfoundation/Streamer.h>

using namespace kfoundation;

class Direction2D : public Streamer {

// --- NESTED TYPES --- //
  
  enum direction_t {
    BACK = -1,
    NEUTRAL = 0,
    FORTH = 1
  };
  

// --- STATIC FIELDS --- //
  
  public: static const Direction2D UP;
  public: static const Direction2D UPPER_RIGHT;
  public: static const Direction2D RIGHT;
  public: static const Direction2D LOWER_RIGHT;
  public: static const Direction2D DOWN;
  public: static const Direction2D LOWER_LEFT;
  public: static const Direction2D LEFT;
  public: static const Direction2D UPPER_LEFT;
  public: static const Direction2D CENTER;
  public: static const Direction2D ALL_DIRECTIONS[9];
  
  
// --- STATIC METHODS --- //
  
  static Direction2D fromOrdinal(kf_int8_t ordinal);
  
  
// --- FIELDS --- //
  
  private: kf_int8_t _ordinal;
  private: direction_t _x;
  private: direction_t _y;
  
  
// --- CONSTRUCTOR --- //
  
  private: Direction2D(direction_t x, direction_t y, kf_int8_t ordinal);
  public: Direction2D();
  

// --- METHODS --- //
  
  public: kf_int8_t getOrdinal() const;
  public: bool isNorth() const;
  public: bool isSouth() const;
  public: bool isEast() const;
  public: bool isWest() const;
  public: bool isCenter() const;
  
  // From Streamer
  void printToStream(ostream& os) const;
  

// --- OPERATORS --- //
  
  bool operator==(const Direction2D& other) const;
  
  
};

#endif /* defined(__ARE__Direction2D__) */
