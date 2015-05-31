//
//  CircleUiAgent.h
//  ARE
//
//  Created by Hamed KHANDAN on 10/16/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef __ARE__CircleUiAgent__
#define __ARE__CircleUiAgent__

#include <knorba/Agent.h>
#include <knorba/Group.h>
#include <knorba/type/all.h>

using namespace knorba;

class CircleUiAgent : public Agent {
public:
  static const SPtr<KString> OP_INIT;
  static const SPtr<KString> OP_QUIT;
  static const SPtr<KString> OP_AREA_Q;
  static const SPtr<KString> OP_AREA_A;
  static const SPtr<KString> OP_DRAW;
  static const SPtr<KString> OP_DISPLAY_INFO_QUERY;
  static const SPtr<KString> OP_DISPLAY_INFO_INFORM;
  
private:
  int  _pipe[2];
  int  _childId;
  bool _startingUi;
  int  _translateX;
  int  _translateY;
  Ptr<Rect> _displayRect;
  
public:
  CircleUiAgent(const k_guid_t& guid, const k_integer_t cloneId);
  ~CircleUiAgent();
  
private:
  void startUi(int x, int y, int w, int h);
  
public:
  void handleMessage(PPtr<Message> msg);
  void quit();
  
};

extern "C" void init();
extern "C" knorba::Agent* instantiate(const k_guid_t& guid, const k_integer_t cloneId);

#endif /* defined(__ARE__CircleUiAgent__) */
