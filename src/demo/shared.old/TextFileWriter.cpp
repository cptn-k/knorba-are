//
//  TextFileWriter.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 10/20/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#include "TextFileWriter.h"

#include <fstream>
#include <knorba/Runtime.h>

#include "TextFileWriter.h"

using namespace std;

const StaticPtr<KString> TextFileWriter::OP_SET_FILENAME(new KString("knorba.demo.textfilewriter.set_filename"));
const StaticPtr<KString> TextFileWriter::OP_PRINT(new KString("knorba.demo.text.print"));


TextFileWriter::TextFileWriter(const k_guid_t guid, const k_integer_t cloneId)
: Agent(guid, cloneId)
{
  registerHandler((handler)&TextFileWriter::opPrintHandler, OP_PRINT);
  registerHandler((handler)&TextFileWriter::opSetFilenameHandler, OP_SET_FILENAME);
  setAutoExit();
}


void TextFileWriter::opSetFilenameHandler(Ptr<Message> msg) {
  fileName = msg->getPayload().AS(KString)->toUtf8String();
}


void TextFileWriter::opPrintHandler(Ptr<Message> msg) {
  if(fileName.length() == 0) {
    return;
  }
  
  ofstream ofs("hello.txt");
  ofs << *msg->getPayload().AS(KString) << endl;
  ofs.close();
}


KF_EXPORT
void init() {
  Runtime::get().registerMessageFormat(TextFileWriter::OP_PRINT, KType::STRING);
  Runtime::get().registerMessageFormat(TextFileWriter::OP_SET_FILENAME, KType::STRING);
}


KF_EXPORT
Agent* instantiate(k_guid_t guid, k_integer_t cloneId) {
  return new TextFileWriter(guid, cloneId);
}