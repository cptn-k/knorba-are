//
//  BubblesAgent.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 10/15/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#include <cstdlib>
#include <unistd.h>

#include <kfoundation/Logger.h>
#include <knorba/Runtime.h>
#include <knorba/MessageSet.h>
#include "BubblesAgent.h"

#define BUBBLE_CREATION_CHANCE 50

using namespace std;


//\/ Bubble /\/////////////////////////////////////////////////////////////////

// -- STATIC FIELDS -- //

SPtr<KRecordType> Bubble::TYPE;
SPtr<KGridType> Bubble::GRID_TYPE;


// -- STATIC METHODS -- //

PPtr<KRecordType> Bubble::type() {
  if(TYPE.isNull()) {
    TYPE = SPtr<KRecordType>(new KRecordType("Bubble"));
    TYPE->addMember("circle", Circle::type().AS(KType));
    TYPE->addMember("vx", KType::REAL);
    TYPE->addMember("vy", KType::REAL);
    TYPE->addMember("vr", KType::REAL);
  }
  
  return TYPE;
}

PPtr<KGridType> Bubble::grid_type() {
  if(GRID_TYPE.isNull()) {
    GRID_TYPE = new KGridType(type().AS(KType), 1);
  }
  return GRID_TYPE;
}


// -- CONSTRUCTORS -- //

Bubble::Bubble()
{
  _value = type()->instantiateVal().AS(KRecord);
  _circle = new Circle(_value->field(0).AS(KRecord));
}


Bubble::Bubble(PPtr<KRecord> value)
{
  _value = value;
  _circle = new Circle(_value->field(0).AS(KRecord));
}


Bubble::~Bubble() {
  // Nothing;
}


// -- METHODS -- //

void Bubble::setVX(const k_real_t vx) {
  _value->field(1).AS(KReal)->set(vx);
}


void Bubble::setVY(const k_real_t vy) {
  _value->field(2).AS(KReal)->set(vy);
}


void Bubble::setVR(const k_real_t vr) {
  _value->field(3).AS(KReal)->set(vr);
}


k_real_t Bubble::getVX() const {
  return _value->field(1).AS(KReal)->get();
}


k_real_t Bubble::getVY() const {
  return _value->field(2).AS(KReal)->get();
}


k_real_t Bubble::getVR() const {
  return _value->field(3).AS(KReal)->get();
}


PPtr<Circle> Bubble::getCircle() const {
  return _circle;
}


PPtr<KValue> Bubble::getKValue() const {
  return _value.AS(KValue);
}


void Bubble::evaluate(const k_real_t dt) {
//  _circle->setX(_circle->getX() + getVX()*dt);
//  _circle->setY(_circle->getY() + getVY()*dt);
  _circle->setY(_circle->getY() - 10);
  _circle->setX(_circle->getX() + 5 - random()%10);
  if(_circle->getR() < 30) {
    _circle->setR(_circle->getR() + 3);
  } else {
    _circle->setR(_circle->getR() + 2);
  }
}


void Bubble::serialize(PPtr<kfoundation::ObjectSerializer> serializer) const {
  _value->serialize(serializer);
}



//\/ BubbleAgent /\////////////////////////////////////////////////////////////

// -- GLOBAL -- //

void printCircle(stringstream& sstr, PPtr<KRecord> r, const int tx, const int ty) {
  sstr << ((int)r->field(0).AS(KReal)->get() - tx);
  sstr.put('x');
  sstr << ((int)r->field(1).AS(KReal)->get() - ty);
  sstr.put('y');
  sstr << (int)r->field(2).AS(KReal)->get();
  sstr.put('r');
  sstr.put('a');
}

void* runner(void* args) {
  BubbleAgent* agent = (BubbleAgent*)args;
  
  LOG << "Thread started." << EL;
  cout << LongInt::toHexString(agent) << ", "
       << LongInt::toHexString(&agent->_count) << endl;
  
  int n = 0;
  int x = 0;
  int stepsTillNextBubble = 2;
  
  timeval start;
  timeval now;
  gettimeofday(&start, NULL);
  
  Array<int> toDelete;
  
  int translateX = agent->_displayArea->getLeft();
  int translateY = agent->_displayArea->getTop();

  while(!agent->_stopThread) {
    
    usleep(10000);
    
    stringstream sstr;
    sstr.put('c');

    if(agent->_isAtBottom) {
      if(n == 0) {
        
        int number = rand()%100;
        bool createNewBubbles = (number < BUBBLE_CREATION_CHANCE);
        if(createNewBubbles) {
          n = rand()%10;
          x = rand()%agent->_displayArea->getWidth();
        }
        
      } else {
        
        stepsTillNextBubble--;
        if(stepsTillNextBubble <= 0) {
          Ptr<Bubble> bubble = new Bubble();
          bubble->setVR(0);
          bubble->setVX(0);
          bubble->setVY(-10);
          bubble->getCircle()->setR(2);
          bubble->getCircle()->setX(x + 10 - random()%40);
          bubble->getCircle()->setY(agent->_displayArea->getBottom());
          
          pthread_mutex_lock(&agent->_mutex);
          agent->_bubbles->push(bubble);
          pthread_mutex_unlock(&agent->_mutex);
          
          n--;
          stepsTillNextBubble = 4;
          
          agent->_count++;
        }
        
      }
    }
    
    if(agent->_neighbours->getCount() > 0) {
      Ptr<MessageSet> responses = agent->tsend(
          agent->_neighbours,
          BubbleAgent::OP_QUERY_Q,
          agent->_displayArea.AS(KValue),
          100000);
      
      if(responses->getSize() < agent->_neighbours->getCount()) {
        agent->_neighbours = responses->getSenders();
      }
      
      for(int i = responses->getSize() - 1; i >= 0; i--) {
        PPtr<Message> response = responses->get(i);
        
        Ptr<KGridBasic> remoteCircles = response->getPayload().AS(KGridBasic);
        int s = remoteCircles->getDimensions().get(0);
        Ptr<KRecord> binder = Circle::type()->instantiateRef().AS(KRecord);
        for(int i = 0; i < s; i++) {
          remoteCircles->bind(binder.AS(KValue), i);
          printCircle(sstr, binder, translateX, translateY);
        }
      }
    }
    
    
    pthread_mutex_lock(&agent->_mutex);
    int s = agent->_bubbles->getSize();
    for(int i = 0; i < s; i++) {
      if(!agent->_mark[i]) {
        continue;
      }
      
      PPtr<Bubble> bubble = agent->_bubbles->at(i);
      
      gettimeofday(&now, NULL);
      int diff = (int)(now.tv_sec - start.tv_sec) * 1000000
               + now.tv_usec - start.tv_usec;
      
      agent->_bubbles->at(i)->evaluate((k_real_t)diff / 1000000);
      
      printCircle(sstr, agent->_bubbles->at(i)->getCircle()->getKValue(), translateX, translateY);
      
      if(agent->_mark[i] &&
         (bubble->getCircle()->getY() - bubble->getCircle()->getR())
           <= agent->_displayArea->getTop())
      {
        if(!agent->_isOnTop) {
          agent->send(agent->_above, BubbleAgent::OP_HANDOVER, bubble->getKValue());
        }
        agent->_mark[i] = false;
        //agent->_count--;
      }
    }
    
    for(int i = toDelete.getSize() - 1; i >= 0; i--) {
      agent->_bubbles->remove(toDelete.get(i));
      //agent->_count--;
    }
    
    toDelete.clear();
    pthread_mutex_unlock(&agent->_mutex);
    
    gettimeofday(&start, NULL);
    
    // BEGIN draw
    if(System::getPipes() != NULL) {
      sstr.put('\n');
      string text = sstr.str();
      if(text.length() > 2){
        write(System::getPipes()[WRITE_END], text.c_str(), text.size());
      }
    }
    // END draw
    
  }
  
  LOG << "Thread stopped." << EL;
  
  return NULL;
}


// -- STATIC FIELDS -- //

const SPtr<KString> BubbleAgent::OP_LINK_OUTPUT
    = new KStringVal("bubbleagent.link_output");

const SPtr<KString> BubbleAgent::OP_HELLO = new KStringVal("bubbleagent.hello");
const SPtr<KString> BubbleAgent::OP_QUERY_Q = new KStringVal("bubbleagent.query_q");
const SPtr<KString> BubbleAgent::OP_QUERY_A = new KStringVal("bubbleagent.query_a");
const SPtr<KString> BubbleAgent::OP_HANDOVER = new KStringVal("bubbleagent.handover");
const SPtr<KString> BubbleAgent::OP_UI_AREA_Q = new KStringVal("circleui.area_q");
const SPtr<KString> BubbleAgent::OP_UI_DRAW = new KStringVal("circleui.draw");

const SPtr<KString> BubbleAgent::OP_DISPLAY_INFO_QUERY
    = new KStringVal("knorba.displayinfo.query_local");


// -- CONSTRUCTOR -- //

BubbleAgent::BubbleAgent(const k_guid_t& guid, const k_integer_t rank)
: Agent(guid, rank)
{
  _bubbles = new ManagedArray<Bubble>();
  _knownPeers = new Group();
  _neighbours = new Group();
  _isOutputLinked = false;
  _stopThread = false;
  _isOnTop = true;
  _isAtBottom = true;
  
  pthread_mutex_init(&_mutex, NULL);
  _count = 0;
  
  for(int i = 0; i < 1000; i++) {
    _mark[i] = true;
  }
}


BubbleAgent::~BubbleAgent() {
  pthread_mutex_destroy(&_mutex);
}


// -- METHODS -- //

void BubbleAgent::handleMessage(PPtr<Message> msg) {

  if(msg->is(OP_QUERY_Q)) { // ------------------------------------------------
    
    Ptr<Rect> remoteRect = new Rect(msg->getPayload().AS(KRecord));
    
    Ptr< ManagedArray<Circle> > list = new ManagedArray<Circle>();
    
    for(int i = _bubbles->getSize() - 1; i >= 0; i--) {
      PPtr<Bubble> b = _bubbles->at(i);
      if(b->getCircle()->overlapsWith(remoteRect)) {
        list->push(b->getCircle());
      }
    }
    
    Ptr<KGridBasic> circlesGrid = new KGridVal(Circle::grid_type(), Tuple1D(list->getSize()));
    Ptr<KValue> binder = Circle::type()->instantiateRef();
    
    for(int i = list->getSize() - 1; i >= 0; i--) {
      circlesGrid->bind(binder, i);
      binder->set(list->at(i)->getKValue().AS(KValue));
    }
    
    respond(msg, OP_QUERY_A, circlesGrid.AS(KValue));
    
  } else if(msg->is(OP_HANDOVER)) { // ----------------------------------------

    _count++;
    Ptr<Bubble> b = new Bubble(msg->getPayload().AS(KRecord));
    _bubbles->push(b);
    
  } else if(msg->is(OP_HELLO)) { // -------------------------------------------
    
    if(_displayArea.isNull()) {
      LOG << "Display area not available. Waiting." << EL;
      usleep(200000);
    }
    
    const k_guid_t sender = msg->getSender();
    
    if(!_knownPeers->containts(sender)) {
      _knownPeers->add(sender);
      
      log() << "New peer added: " << KGlobalUid::toString(sender) << EL;
      
      Ptr<Rect> rect = new Rect(msg->getPayload().AS(KRecord));
      
      if(rect->isAdjecentTo(_displayArea)) {
        LOG << "Neighbour." << EL;
        
        _neighbours->add(sender);
        
        Direction2D d = rect->getRelativePositionTo(_displayArea);
        
        LOG << "Relative position: " << d << EL;
        
        if(d == Direction2D::UP) {
          _isOnTop = false;
          _above = sender;
          LOG << "Agent above: " << KGlobalUid::toString(sender) << EL;
        } else if(d == Direction2D::DOWN) {
          _isAtBottom = false;
          _below = sender;
          LOG << "Agent below: " << KGlobalUid::toString(sender) << EL;
        }
      }
            
      send(sender, OP_HELLO, _displayArea.AS(KValue));
    }
    
  } else if(msg->is(OP_LINK_OUTPUT)) { // -------------------------------------
    
    Ptr<MessageSet> responses = tsendToLocals(OP_DISPLAY_INFO_QUERY,
                                              KValue::NOTHING, 10000);
    
    while(responses->isEmpty()) {
      cout << "Display info not available. Retrying in 10 seconds." << endl;
      ::sleep(10);
      responses = tsendToLocals(OP_DISPLAY_INFO_QUERY, KValue::NOTHING, 10000);
    }
    
    PPtr<KRecord> config = responses->get(0)->getPayload().AS(KRecord);
    PPtr<KGridBasic> displays = config->field("displays").AS(KGridBasic);
    
    if(displays->getDimensions().get(0) > 0) {
      PPtr<KRecord> display1 = displays->at(Tuple1D(0)).AS(KRecord);
      PPtr<KRecord> size = display1->field("size").AS(KRecord);
      PPtr<KRecord> offset = display1->field("globalPosition").AS(KRecord);
      _displayArea = new Rect();
      _displayArea->setLeft(offset->field(0).AS(KInteger)->get());
      _displayArea->setTop(offset->field(1).AS(KInteger)->get());
      _displayArea->setWidth(size->field(0).AS(KInteger)->get());
      _displayArea->setHeight(size->field(1).AS(KInteger)->get());
      log() << "Display area: " << *_displayArea << EL;
    } else {
      log() << "There are no displays. Quitting." << EL;
      quit();
      return;
    }

    sleep(2000000);
    
    sendToAll(OP_HELLO, _displayArea.AS(KValue));
    
    _thread = System::makeThread(&runner, this);
    
  } // ------------------------------------------------------------------------
  
}


//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

KF_EXPORT
void init() {
  Ptr<KGridType> gridOfCircles(new KGridType(Circle::type().AS(KType), 1));
  
//  Runtime::get().registerType(gridOfCircles.AS(KType));
//  Runtime::get().registerType(Bubble::type().AS(KType));

  Runtime::get().registerMessageFormat(BubbleAgent::OP_LINK_OUTPUT, KType::NOTHING);
  Runtime::get().registerMessageFormat(BubbleAgent::OP_HELLO,       Rect::type().AS(KType));
  Runtime::get().registerMessageFormat(BubbleAgent::OP_QUERY_Q,     Rect::type().AS(KType));
  Runtime::get().registerMessageFormat(BubbleAgent::OP_QUERY_A,     Circle::grid_type().AS(KType));
  Runtime::get().registerMessageFormat(BubbleAgent::OP_HANDOVER,    Bubble::type().AS(KType));
}

KF_EXPORT
Agent* instantiate(const k_guid_t& guid, const k_integer_t cloneId) {
  return new BubbleAgent(guid, cloneId);
}
