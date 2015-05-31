//
//  Bubble.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 11/21/14.
//  Copyright (c) 2014 Kay Khandan. All rights reserved.
//

#ifndef BUBBLE_H2
#define BUBBLE_H2

// KnoRBA
#include <knorba/type/KRecord.h>
#include <knorba/type/KReal.h>

// Internal
#include "Circle.h"

using namespace knorba::type;

class Bubble : public ManagedObject, public SerializingStreamer {
  
// --- STATIC FIELDS --- //
  
  private: static SPtr<KRecordType> TYPE;
  
  
// --- STATIC METHODS --- //
  
  public: static PPtr<KRecordType> type();
  
  
// --- FIELDS --- //

  private: Ptr<KRecord> _record;
  private: Ptr<Circle>  _circle;
  
  
// --- (DE)CONSTRUCTOR --- //
  
  public: Bubble();
  public: Bubble(PPtr<KRecord> rec);
  public: ~Bubble();
  
  
// --- METHODS --- //
  
  public: void wrap(PPtr<KRecord> rec);
  public: void wrap(PPtr<KGridBasic> grid, Tuple index);
  public: PPtr<Circle> getCircle() const;
  public: k_real_t getVX() const;
  public: k_real_t getVY() const;
  public: k_real_t getVR() const;
  public: void setVX(const k_real_t vx);
  public: void setVY(const k_real_t vy);
  public: void setVR(const k_real_t vr);
  
  // Inherited from SerializingStreamer
  public: void serialize(PPtr<ObjectSerializer> serializer) const;
  
};


#endif /* defined(BUBBLE_H2) */
