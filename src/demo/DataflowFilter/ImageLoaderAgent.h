//
//  ImageLoaderAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Kay Khandan on 5/29/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__ImageLoaderAgent__
#define __AgentRuntimeEnvironment__ImageLoaderAgent__

#include <knorba/Agent.h>

using namespace knorba;
using namespace knorba::type;

class ImageLoaderAgent: public Agent{

private:
  static SPtr<KRecordType> PIXEL_T;
  static SPtr<KGridType> IMAGE_T;
  
public:
  static const SPtr<KString> OP_DATA_Q;
  static const SPtr<KString> OP_DATA_A;
  static const SPtr<KString> OP_SETFILE;
  static const SPtr<KString> R_INPUT;
  
public:
  static SPtr<KRecordType> pixel_t();
  static SPtr<KGridType> image_t();

private:
  Ptr<KString> _fileName;
  
  ImageLoaderAgent(Runtime &rt, k_guid_t &guid);
  
public:
  void opSetFile(PPtr<Message>);
  void opDataQ(PPtr<Message>);
};

extern "C" void init(Runtime& rt);
extern "C" knorba::Agent* instantiate(Runtime& rt, const k_guid_t& guid);


#endif /* defined(__AgentRuntimeEnvironment__ImageLoaderAgent__) */