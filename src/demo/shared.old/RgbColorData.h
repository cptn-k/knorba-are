//
//  RGBColorData.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/24/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__RGBColorData__
#define __AgentRuntimeEnvironment__RGBColorData__

#include <knorba/type/KRecord.h>

using namespace knorba::type;

class RgbColorData : public KRecord {
  
// --- STATIC FIELDS --- //
  
  private: static SPtr<KRecordType> TYPE;
  
  
// --- STATIC METHODS --- //
  
  public: static PPtr<KRecordType> type();
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: RgbColorData();
  public: RgbColorData(k_octet_t r, k_octet_t g, k_octet_t b);
  public: ~RgbColorData();
  
  
// --- METHODS --- //
  
  public: void set(k_octet_t r, k_octet_t g, k_octet_t b);
  public: void setR(k_octet_t r);
  public: void setG(k_octet_t g);
  public: void setB(k_octet_t b);
  public: k_octet_t getR() const;
  public: k_octet_t getG() const;
  public: k_octet_t getB() const;
  
};

#endif /* defined(__AgentRuntimeEnvironment__RGBColorData__) */
