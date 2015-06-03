//
//  PicProtocol.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 5/29/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__PicProtocol__
#define __AgentRuntimeEnvironment__PicProtocol__

// KnoRBA
#include <knorba/Protocol.h>

using namespace knorba;
using namespace knorba::type;

class PicProtocol : Protocol {
  
// --- STATIC FIELDS --- //
  
  private: static SPtr<KRecordType> LOAD_T;
  public: static const k_octet_t LOAD_T_RELAY = 0;
  public: static const k_octet_t LOAD_T_REFERENCE = 1;
  public: static const k_octet_t LOAD_T_FILENAME = 2;
  public: static const k_octet_t LOAD_T_DATA = 3;
  
  private: static SPtr<KRecordType> PUT_T;
  public: static const k_octet_t PUT_T_RELAY = 0;
  public: static const k_octet_t PUT_T_REFERENCE = 1;
  public: static const k_octet_t PUT_T_AREA = 2;
  
  private: static SPtr<KRecordType> UNPUT_T;
  public: static const k_octet_t UNPUT_T_RELAY = 0;
  public: static const k_octet_t UNPUT_T_REFERENCE = 1;
  
  public: static const SPtr<KString> OP_LOAD;
  public: static const SPtr<KString> OP_PUT;
  public: static const SPtr<KString> OP_UNPUT;
  public: static const SPtr<KString> OP_CLEAR;
  public: static const SPtr<KString> OP_QUIT;
  

// --- STATIC METHODS --- //
  
  public: static SPtr<KRecordType> load_t();
  public: static SPtr<KRecordType> put_t();
  public: static SPtr<KRecordType> unput_t();
  public: static void init(Runtime& r);
  
};

#endif /* defined(__AgentRuntimeEnvironment__PicProtocol__) */