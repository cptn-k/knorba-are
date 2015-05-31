//
//  Circle.h
//  ARE
//
//  Created by Hamed KHANDAN on 10/17/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef __ARE__Circle__
#define __ARE__Circle__

#include <kfoundation/ManagedObject.h>
#include <knorba/type/all.h>

using namespace kfoundation;
using namespace knorba;
using namespace knorba::type;

class Rect;

class Circle : public ManagedObject, public SerializingStreamer {
  
// --- STATIC FIELDS --- //
  
  public: static SPtr<KRecordType> TYPE;
  public: static SPtr<KGridType> GRID_TYPE;
  
  
// --- STATIC METHODS --- //
  
  public: static PPtr<KRecordType> type();
  public: static PPtr<KGridType> grid_type();
  
  
// --- FIELDS --- //
  
  Ptr<KRecord> _value;
  
  
// --- CONSTRUCTORS --- //
  
  public: Circle();
  public: Circle(PPtr<KRecord> value);
  public: ~Circle();
  
  
// --- METHODS --- //
  
  private: static bool isInRange(k_real_t n, k_real_t a, k_real_t b);
  public: void set(Ptr<KRecord> value);
  public: void set(k_real_t x, k_real_t y, k_real_t r);
  public: void setX(k_real_t x);
  public: void setY(k_real_t y);
  public: void setR(k_real_t r);
  public: k_real_t getX() const;
  public: k_real_t getY() const;
  public: k_real_t getR() const;
  public: bool overlapsWith(PPtr<Circle> other) const;
  public: bool overlapsWith(PPtr<Rect> rect) const;
  public: PPtr<KRecord> getKValue() const;
  
  // From Serializing Streamer
  public: void serialize(PPtr<ObjectSerializer> builder) const;
  
};

#endif /* defined(__ARE__Circle__) */
