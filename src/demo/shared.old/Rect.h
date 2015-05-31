//
//  Rect.h
//  ARE
//
//  Created by Hamed KHANDAN on 10/17/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef __ARE__Rect__
#define __ARE__Rect__

// KFoundation
#include <kfoundation/Range.h>

// KnoRBA
#include <knorba/type/KRecord.h>

using namespace kfoundation;
using namespace knorba::type;


class Rect : public KRecord {
  
// --- STATIC FIELDS --- //
  
  private: static SPtr<KRecordType> TYPE;
  
  
// --- STATIC METHODS --- //
  
  public: static Ptr<KRecordType> type();
  
  
// --- CONSTRUCTORS --- //
  
  public: Rect();
  public: Rect(const Range& v);
  
  
// --- METHODS --- //
  
  public: void set(const Range& value);
  public: Range get() const;
    
};

#endif /* defined(__ARE__Rect__) */
