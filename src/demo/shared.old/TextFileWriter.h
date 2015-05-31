//
//  TextFileWriter.h
//  ARE
//
//  Created by Hamed KHANDAN on 10/20/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef ARE_DEMO_TEXTFILEWRITER
#define ARE_DEMO_TEXTFILEWRITER


#include <knorba/Agent.h>
#include <knorba/type/all.h>

using namespace knorba;
using namespace knorba::type;


class TextFileWriter : public Agent {
  
// --- STATIC FIELS --- //
  
  public: const static StaticPtr<KString> OP_SET_FILENAME;
  public: const static StaticPtr<KString> OP_PRINT;
  
  
// --- FIELDS --- //
  
  private: string fileName;
  
  
// --- CONSTRUCTOR --- //
  
  public: TextFileWriter(const k_guid_t guid, const k_integer_t cloneId);
  
  
// --- METHODS --- //
  
  public: void opSetFilenameHandler(Ptr<Message> msg);
  public: void opPrintHandler(Ptr<Message> msg);
  
};


extern "C" void init();
extern "C" knorba::Agent* instantiate(k_guid_t guid, k_integer_t cloneId);


#endif /* defined(ARE_DEMO_TEXTFILEWRITER) */
