//
//  GridOutputProtocol.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/6/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__GridOutputProtocol__
#define __AgentRuntimeEnvironment__GridOutputProtocol__

#include <knorba/Protocol.h>
#include <knorba/Runtime.h>
#include <knorba/type/all.h>

using namespace knorba;
using namespace knorba::type;

class PixmapOutputProtocol : public Protocol {
  
// --- NESTED TYPES --- //
  
  public: typedef enum {
    BLACK_AND_WHITE = 0,
    GRAY_16         = 1,
    GRAY_256        = 2,
    COLOR_16        = 3
  } output_format_t;
  
  
// --- STATIC FIELDS --- //
  
  private: static SPtr<KEnumerationType> FORMAT_T;
  private: static SPtr<KGridType> GRID_T;
  private: static SPtr<KRecordType> PUT_T;
  public: static const k_octet_t PUT_T_FORMAT = 0;
  public: static const k_octet_t PUT_T_PHASE  = 1;
  public: static const k_octet_t PUT_T_DATA   = 2;
  
  public: static SPtr<KString> OP_PUT;
  
  
// --- STATIC METHODS --- //
  
  public: static PPtr<KEnumerationType> format_t();
  public: static PPtr<KGridType> grid_t();
  public: static PPtr<KRecordType> put_t();
  public: static void init(Runtime& rt);
  
  
// --- FIELDS --- //
  
  private: Ptr<Group> _targets;
  private: Ptr<KRecord> _payload;
  private: Ptr<KString> _serverRole;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: PixmapOutputProtocol(Agent* owner, output_format_t outputFormat,
      PPtr<KString> serverRole);
  
  
// --- METHODS --- //
  
  public: void setFormat(output_format_t f);
  public: void put(k_integer_t phase, PPtr<KGrid> data);
  public: void tput(k_integer_t phase, PPtr<KGrid> data);
};

#endif /* defined(__AgentRuntimeEnvironment__GridOutputProtocol__) */