//
//  FileGridOutputAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Kay Khandan on 3/8/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// Std
#include <fstream>

// Self
#include "FileGridOutputAgent.h"

using namespace std;

//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

KF_EXPORT
void init(Runtime& rt) {
  rt.registerMessageFormat(FileGridOutputAgent::OP_SET_PATH, KType::STRING);
  PixmapOutputProtocol::init(rt);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new FileGridOutputAgent(rt, guid);
}


//\/ FileGridOutputAgent /\////////////////////////////////////////////////////

// --- STATIC FIELDS --- //

SPtr<KString> FileGridOutputAgent::OP_SET_PATH
    = new KString("knorba.demo.file-grid-output.set-path");


// --- (DE)CONSTRUCTORS --- //

FileGridOutputAgent::FileGridOutputAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid)
{
  // Nothing;
}


void FileGridOutputAgent::write(PixmapOutputProtocol::output_format_t format,
    k_integer_t phase, PPtr<KGrid> data, ostream& os)
{
  os << format << ' ' << phase << endl;
  
  const Tuple& size = data->getSize();
  
  Tuple2D coords(0, 0);
  Ptr<KRecord> r = new KRecord(data);
  for(int i = 0; i < size.at(0); i++) {
    for(int j = 0; j < size.at(1); j++) {
      k_octet_t v = data->at(coords.set(i, j), r)->getOctet();
      
      switch (format) {
        case PixmapOutputProtocol::BLACK_AND_WHITE:
          if(v == 0) {
            os << '.';
          } else {
            os << 'O';
          }
          break;
          
        case PixmapOutputProtocol::COLOR_16:
        case PixmapOutputProtocol::GRAY_16:
          os << Int::toHexString(v);
          break;
          
        case PixmapOutputProtocol::GRAY_256:
          os << (int)v << ' ';
          break;
      }
      
    } // for(j)
    os << endl;
    
  } // for(i);
  os << endl;
}


void FileGridOutputAgent::handleMessage(PPtr<Message> message) {
  
  if(message->is(PixmapOutputProtocol::OP_PUT)) {
  
    PPtr<KRecord> r = message->getPayload().AS(KRecord);
    
    PixmapOutputProtocol::output_format_t format
        = (PixmapOutputProtocol::output_format_t)r->getEnumerationOrdinal(0);
    
    k_integer_t phase = r->getInteger(1);
    
    if(_dirPath.isNull()) {
      write(format, phase, r->field<KGrid>(2), cout);
    } else {
      Ptr<Path> filePath = _dirPath->addSegement(Int::toString(phase) + ".txt");
      ofstream ofs(filePath->getString().c_str());
      write(format, phase, r->field<KGrid>(2), ofs);
      ofs.close();
    }
    
  } else if(message->is(PixmapOutputProtocol::OP_PING_Q)) {
    
    respond(message, PixmapOutputProtocol::OP_PING_A, KValue::NOTHING);
    
  } else if(message->is(OP_SET_PATH)) {
  
    _dirPath = new Path(message->getPayload().AS(KString)->toUtf8String());
    if(!_dirPath->exists()) {
      _dirPath->makeDir();
    }
    
  }
  
}

