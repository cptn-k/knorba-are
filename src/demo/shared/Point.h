//
//  Point.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/29/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__Point__
#define __AgentRuntimeEnvironment__Point__

// Super
#include <kfoundation/Tuple.h>

namespace knorba {
namespace demo {
  
  using namespace kfoundation;
  using namespace knorba::type;
  
  class Point : public Tuple2D {
  
  // --- STATIC FIELDS --- //
    
    private: static SPtr<KRecordType> TYPE;
    private: static const k_octet_t X = 0;
    private: static const k_octet_t Y = 1;
    
    
  // --- STATIC METHODS --- //
    
    public: static SPtr<KRecordType> type();
    public: static void tupleToKRecord(const Tuple& t, PPtr<KRecord> r);
    
    
  // --- (DE)CONSTRUCTORS --- //
    
    public: Point();
    public: Point(const Tuple& other);
    public: Point(int x, int y);
    public: Point(PPtr<KRecord> r);
    
    
  // --- METHODS --- //
    
    public: void toKRecord(PPtr<KRecord> r) const;
    
  };
  
  
} // namespace demo
} // namespace knorba


#endif /* defined(__AgentRuntimeEnvironment__Point__) */
