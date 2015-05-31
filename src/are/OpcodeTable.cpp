//
//  OpcodeTable.cpp
//  ARE
//
//  Created by Kay Khandan on 10/11/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#include <kfoundation/Ptr.h>
#include <kfoundation/LongInt.h>
#include <kfoundation/Logger.h>
#include "OpcodeTable.h"


namespace knorba {
namespace are {

  OpcodeTable::Item::Item(PPtr<KString> o, PPtr<KType> t) {
    opcode = o;
    type = t;
  }
  
  
  OpcodeTable::OpcodeTable() {
    _items = new ManagedArray<Item>();
  }
  
  
  int OpcodeTable::getIndexForOpcode(const k_longint_t hash) {
    hashmap_t::iterator it = _opcodeMap.find(hash);
    if(it == _opcodeMap.end()) {
      return -1;
    }
    return it->second;
  }
  
  
  void OpcodeTable::registerRecord(PPtr<KString> opcode,
      PPtr<KType> type)
  {
    k_longint_t hash = opcode->getHashCode();
    int index = getIndexForOpcode(hash);
    
    if(index >= 0) {
      return;
    }
    
    index = _items->getSize();
    _items->push(new Item(opcode, type));
    _opcodeMap[opcode->getHashCode()] = index;
    _nameMap[type->getTypeNameHash()] = index;

    LOG << "MESSAGE " << *opcode << " (hash:" << opcode->getHashCode()
        << ") CARRIES " << *type << " (hash:" << type->getTypeNameHash() << ")"
        << EL;
  }
  
  
  PPtr<KType> OpcodeTable::getTypeByOpcodeHash(const k_longint_t hash) {
    hashmap_t::iterator it = _opcodeMap.find(hash);
    if(it == _opcodeMap.end()) {
      return NULL;
    }
    return _items->at(it->second)->type;
  }
  
  
  PPtr<KType> OpcodeTable::getTypeByNameHash(const k_longint_t hash) {
    hashmap_t::iterator it = _nameMap.find(hash);
    if(it == _nameMap.end()) {
      return NULL;
    }
    return _items->at(it->second)->type;
  }
  
  
  PPtr<KString> OpcodeTable::getOpcode(const k_longint_t &hash) {
    hashmap_t::iterator it = _opcodeMap.find(hash);
    if(it == _opcodeMap.end()) {
      return NULL;
    }
    return _items->at(it->second)->opcode;
  }

  
  void OpcodeTable::serialize(PPtr<ObjectSerializer> builder) const {
    builder->object("OpcodeTable");
    for(int i = 0; i < _items->getSize(); i++) {
      PPtr<Item> item = _items->at(i);
      builder->object("Item")
        ->attribute("opcode", item->opcode->toUtf8String())
        ->member("type")->object<KType>(item->type)
        ->endObject();
    }
    builder->endObject();
  }

} // namespace are
} // namespace knorba