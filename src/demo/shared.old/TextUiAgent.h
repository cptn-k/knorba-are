//
//  TextUiAgent.h
//  ARE
//
//  Created by Hamed KHANDAN on 10/10/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef __ARE__TextUiAgent__
#define __ARE__TextUiAgent__

#include <knorba/Agent.h>
#include <knorba/Group.h>
#include <knorba/type/all.h>

using namespace knorba;

class TextUiAgent : public Agent {

// --- STATIC FIELDS --- //
  
  public: static const StaticPtr<KString> OP_SAY;
  public: static const StaticPtr<KString> OP_INIT;
  public: static const StaticPtr<KString> OP_QUIT;
  public: static const StaticPtr<KString> OP_DISPLAYINFO_QUERY;

  
// --- FIELDS --- //
  
  public: int  _pipe[2];
  public: int  _childId;
  public: bool _startingUi;


// --- (DE)CONSTRUCTORS --- //
  
  public: TextUiAgent(k_guid_t guid, k_integer_t cloneId);
  public: ~TextUiAgent();
  
  
// --- METHODS --- //
  
  private: void startUi(int x, int y, int w, int h);
  private: void handleMessage(Ptr<Message> msg);
  
  // Inherited from Agent
  void quit();
  
};

extern "C" void init();
extern "C" knorba::Agent* instantiate(k_guid_t guid, k_integer_t cloneId);

#endif /* defined(__ARE__TextUiAgent__) */
