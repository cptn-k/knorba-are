//
//  PixelData.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 2/27/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__PixelData__
#define __AgentRuntimeEnvironment__PixelData__

// KnoRBA
#include <knorba/type/KRecord.h>

// Internal
#include "RgbColorData.h"
#include "Location2D.h"

using namespace knorba::type;

class PixelData : public KRecord {
  
// --- STATIC FIELDS --- //
  
  private: static SPtr<KRecordType> TYPE;
  
  
// --- STATIC METHODS --- //
  
  public: static PPtr<KRecordType> type();
  
  
// --- FIELDS --- //
  
  private: Ptr<Location2D> _location;
  private: Ptr<RgbColorData> _color;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: PixelData();
  public: ~PixelData();
  
  
// --- METHODS --- //
  
  public: PPtr<Location2D> location() const;
  public: PPtr<RgbColorData> color() const;
  
};


#endif /* defined(__AgentRuntimeEnvironment__PixelData__) */
