//
//  BubblesAgent.h
//  ARE
//
//  Created by Hamed KHANDAN on 10/15/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef __ARE__BubblesAgent__
#define __ARE__BubblesAgent__

#include <knorba/Agent.h>
#include <knorba/type/all.h>

#include "Rect.h"
#include "Circle.h"


using namespace kfoundation;
using namespace knorba;
using namespace knorba::type;


//\/ Bubble /\//////////////////////////////////////////////////////////////////


class Bubble : public ManagedObject, public SerializingStreamer {
  
// -- STATIC FIELDS -- //
  
  private: static SPtr<KRecordType> TYPE;
  private: static SPtr<KGridType> GRID_TYPE;
  

// -- STATIC METHODS -- //
  
  public: static PPtr<KRecordType> type();
  public: static PPtr<KGridType> grid_type();
  
  
// -- FIELDS -- //
  
  private: Ptr<KRecord> _value;
  private: Ptr<Circle> _circle;
  
  
// -- CONSTRUCTORS -- //
  
  public: Bubble();
  public: Bubble(PPtr<KRecord> value);
  public: ~Bubble();
  
  
// -- METHODS -- //
  
  public: void set(PPtr<KRecord> value);
  public: void setVX(const k_real_t vx);
  public: void setVY(const k_real_t vy);
  public: void setVR(const k_real_t vr);
  public: k_real_t getVX() const;
  public: k_real_t getVY() const;
  public: k_real_t getVR() const;
  
  public: PPtr<Circle> getCircle() const;
  
  public: void evaluate(const k_real_t dt);
  
  public: PPtr<KValue> getKValue() const;
  public: void serialize(PPtr<ObjectSerializer> serializer) const;
  
};



//\/ BubbleAgent /\////////////////////////////////////////////////////////////


class BubbleAgent : public Agent {
  
// -- FRIENDS -- //
  
  friend void* runner(void* args);
  
  
// -- STATIC FIELDS -- //
  
  public: static const SPtr<KString> OP_LINK_OUTPUT;
  public: static const SPtr<KString> OP_HELLO;
  public: static const SPtr<KString> OP_QUERY_Q;
  public: static const SPtr<KString> OP_QUERY_A;
  public: static const SPtr<KString> OP_HANDOVER;
  public: static const SPtr<KString> OP_UI_AREA_Q;
  public: static const SPtr<KString> OP_UI_DRAW;
  public: static const SPtr<KString> OP_DISPLAY_INFO_QUERY;
  
  
// -- FIELDS -- //
  
  private: pthread_t       _thread;
  private: pthread_mutex_t _mutex;
  
  private: Ptr<Group> _knownPeers;
  private: Ptr<Group> _neighbours;
  private: Ptr<Rect>  _displayArea;
  private: k_guid_t      _output;
  private: bool       _isOutputLinked;
  private: bool       _stopThread;
  private: bool       _isOnTop;
  private: bool       _isAtBottom;
  private: k_guid_t      _above;
  private: k_guid_t      _below;
  private: bool       _mark[1000];
  private: int        _count;
  private: Ptr< ManagedArray<Bubble> > _bubbles;
  
  
// -- CONSTRUCTOR -- //
  
  public: BubbleAgent(const k_guid_t& guid, const k_integer_t rank);
  public: ~BubbleAgent();
  

// -- METHODS -- //
  
  public: void handleMessage(PPtr<Message> msg);
  
};



//// GLOBAL ///////////////////////////////////////////////////////////////////

extern "C" void init();
extern "C" knorba::Agent* instantiate(const k_guid_t& guid, const k_integer_t cloneId);



//// END //////////////////////////////////////////////////////////////////////


#endif /* defined(__ARE__BubblesAgent__) */
