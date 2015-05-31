//
//  Location2D.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/27/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__Location2D__
#define __AgentRuntimeEnvironment__Location2D__

// KnoRBA
#include <knorba/type/KRecord.h>

using namespace knorba::type;

class Location2D : public KRecord {
  
// --- STATIC FIELDS --- //
  
  private: static SPtr<KRecordType> TYPE;
  
  
// --- STATIC METHODS --- //
  
  public: static PPtr<KRecordType> type();
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: Location2D();
  public: Location2D(const k_integer_t x, const k_integer_t y);
  public: ~Location2D();
  
  
// --- METHODS --- //
  
  public: void set(const k_integer_t x, const k_integer_t y);
  public: void setX(const k_integer_t x);
  public: void setY(const k_integer_t y);
  public: k_integer_t getX() const;
  public: k_integer_t getY() const;
  
};

#endif /* defined(__AgentRuntimeEnvironment__Location2D__) */
