//
//  Circle.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 11/21/14.
//  Copyright (c) 2014 Kay Khandan. All rights reserved.
//

#ifndef CIRCLE_H2
#define CIRCLE_H2

// KnoRBA
#include <knorba/type/KRecord.h>

using namespace knorba::type;

class Circle : public KRecordWrapper {
  
// --- STATIC FIELDS --- //
  
  private: static SPtr<KRecordType> TYPE;
  
  
// --- STATIC METHODS --- //
  
  public: static PPtr<KRecordType> type();
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: Circle();
  public: ~Circle();
  
  
// --- METHODS --- //
  
  public: k_integer_t getX() const;
  public: k_integer_t getY() const;
  public: k_integer_t getR() const;
  
  public: void setX(const k_integer_t x);
  public: void setY(const k_integer_t y);
  public: void setR(const k_integer_t z);
  
};

#endif /* defined(CIRCLE_H2) */
