//
//  OpcodeTable.h
//  ARE
//
//  Created by Kay Khandan on 10/11/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef __ARE__MessageDictionary__
#define __ARE__MessageDictionary__

// Std
#include <map>

// KFoundation
#include <kfoundation/ManagedObject.h>
#include <kfoundation/ManagedArray.h>

// KnoRBA
#include <knorba/type/KType.h>
#include <knorba/type/KString.h>

namespace knorba {
  namespace are {
    
    using namespace std;
    using namespace kfoundation;
    using namespace knorba::type;
    
    class OpcodeTable : public ManagedObject {
      
    // --- NESTED TYPES --- //
      
      private: class Item : public ManagedObject {
        public: Ptr<KType> type;
        public: Ptr<KString> opcode;
        public: Item(PPtr<KString> o, PPtr<KType> t);
      };
      
      private: typedef map<k_longint_t, int> hashmap_t;
      
      
    // --- FIELDS --- //
      
      private: Ptr< ManagedArray<Item> > _items;
      private: hashmap_t _opcodeMap;
      private: hashmap_t _nameMap;
      
      
    // --- (DE)CONSTRUCTORS --- //
      
      public: OpcodeTable();
      
      
    // --- METHODS --- //
      
      private: int getIndexForOpcode(const k_longint_t opcode);
      public: void registerRecord(PPtr<KString> opcode, PPtr<KType> type);
      public: PPtr<KType> getTypeByOpcodeHash(const k_longint_t hash);
      public: PPtr<KType> getTypeByNameHash(const k_longint_t hash);
      public: PPtr<KString> getOpcode(const k_longint_t& hash);
      
      // From SerializingStreamer
      void serialize(PPtr<ObjectSerializer> builder) const;
    };
    
  } // are
} // knorba

#endif /* defined(__ARE__MessageDictionary__) */
