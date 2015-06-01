//
//  Rectangle.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/29/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__Rectangle__
#define __AgentRuntimeEnvironment__Rectangle__

// KFoundation
#include <kfoundation/Range.h>

namespace knorba {
namespace demo {
  
  using namespace kfoundation;
  using namespace knorba::type;
  
  class Rectangle : public Range {
    
  // --- STATIC FIELDS --- //
    
    private: static SPtr<KRecordType> TYPE;
    private: static const k_octet_t BEGIN = 0;
    private: static const k_octet_t END = 1;
    

  // --- STATIC METHODS --- //
    
    public: static SPtr<KRecordType> type();
    public: static void rangeToKRecord(const Range& range, PPtr<KRecord> rec);
    
    
  // --- (DE)CONSTRUCTORS --- //
    
    public: Rectangle();
    public: Rectangle(const Range& other);
    public: Rectangle(const Tuple& begin, const Tuple& end);
    public: Rectangle(PPtr<KRecord> r);
    
    
  // --- METHODS --- //
    
    public: void toKRecord(PPtr<KRecord> r) const;
    
  };
  
} // namespace demo
} // namespace knorba


#endif /* defined(__AgentRuntimeEnvironment__Rectangle__) */
