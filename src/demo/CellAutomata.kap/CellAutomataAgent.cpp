//
//  CellAutomataAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Kay Khandan on 2/28/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/RangeIterator.h>
#include <kfoundation/ProximityIterator.h>

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/Group.h>
#include <knorba/type/all.h>

// Protocols
#include <knorba/protocol/GroupingProtocol.h>
#include <knorba/protocol/DisplayInfoProtocol.h>
#include <knorba/protocol/ConsoleProtocolClient.h>
#include "../shared/PixmapOutputProtocol.h"

// Self
#include "CellAutomataAgent.h"


//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

KF_EXPORT
void init(Runtime& rt) {
  rt.registerMessageFormat(CellAutomataAgent::OP_START,
      KPoint::type().AS(KType));
  
  rt.registerMessageFormat(CellAutomataAgent::OP_PARTITION,
      CellAutomataAgent::partition_map_t().AS(KType));
  
  rt.registerMessageFormat(CellAutomataAgent::OP_BORDER,
      CellAutomataAgent::border_t().AS(KType));
  
  rt.registerMessageFormat(CellAutomataAgent::OP_SET_DELAY, KType::INTEGER);
  
  GroupingProtocol::init(rt);
  DisplayInfoProtocol::init(rt);
  ConsoleProtocolClient::init(rt);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new CellAutomataAgent(rt, guid);
}


//\/ KPoint /\/////////////////////////////////////////////////////////////////

SPtr<KRecordType> KPoint::TYPE;


SPtr<KRecordType> KPoint::type() {
  if(TYPE.isNull()) {
    TYPE = new KRecordType("knorba.demo.Point");
    TYPE->addField("x", KType::INTEGER)
        ->addField("y", KType::INTEGER);
  }
  return TYPE;
}


KPoint::KPoint() {
  // Nothing;
}


KPoint::KPoint(const Tuple& other)
: Tuple2D(other.at(0), other.at(1))
{
  // Nothing;
}


KPoint::KPoint(int x, int y)
: Tuple2D(x, y)
{
  // Nothing;
}


KPoint::KPoint(PPtr<KRecord> r)
: Tuple2D(r->getInteger(X), r->getInteger(Y))
{
  // Nothing;
}


void KPoint::toKRecord(PPtr<KRecord> r) const {
  r->setInteger(X, getX());
  r->setInteger(Y, getY());
}


//\/ KRect /\//////////////////////////////////////////////////////////////////

SPtr<KRecordType> KRect::TYPE;


SPtr<KRecordType> KRect::type() {
  if(TYPE.isNull()) {
    TYPE = new KRecordType("knorba.demo.Rect");
    TYPE->addField("begin", KPoint::type().AS(KType))
        ->addField("end", KPoint::type().AS(KType));
  }
  return TYPE;
}


KRect::KRect() {
  // Nothing;
}


KRect::KRect(const Range& other)
: Range(other)
{
  // Nothing;
}


KRect::KRect(const Tuple& begin, const Tuple& end)
: Range(begin, end)
{
  // Nothing;
}


KRect::KRect(PPtr<KRecord> r)
: Range(KPoint(r->getRecord(BEGIN)), KPoint(r->getRecord(END)))
{
  // Nothing;
}


void KRect::toKRecord(PPtr<KRecord> r) const {
  KPoint(getBegin()).toKRecord(r->getRecord(BEGIN));
  KPoint(getEnd()).toKRecord(r->getRecord(END));
}


//\/ CellAutomataAgent::Partition /\///////////////////////////////////////////

SPtr<KRecordType> CellAutomataAgent::Partition::TYPE;


SPtr<KRecordType> CellAutomataAgent::Partition::type() {
  if(TYPE.isNull()) {
    TYPE = new KRecordType("knorba.demo.cell-automata.Partition");
    TYPE->addField("guid", KType::GUID)
        ->addField("rect", KRect::type().AS(KType));
  }
  return TYPE;
}


CellAutomataAgent::Partition::Partition() {
  // Nothing;
}


CellAutomataAgent::Partition::Partition(PPtr<KRecord> r)
: rect(r->getRecord(RECT)),
  guid(r->getGuid(GUID))
{
  // Nothing;
}


void CellAutomataAgent::Partition::toKRecord(PPtr<KRecord> r) const {
  rect.toKRecord(r->getRecord(RECT));
  r->setGuid(GUID, guid);
}


void CellAutomataAgent::Partition::serialize(PPtr<ObjectSerializer> os) const {
  os->object("Partition")
    ->attribute("guid", KGlobalUid::toString(guid))
    ->attribute("rect", rect.toString())
    ->endObject();
}


//\/ CellAutomataAgent::PGrouping /\///////////////////////////////////////////

CellAutomataAgent::PGrouping::PGrouping(CellAutomataAgent* owner)
: GroupingProtocol(owner, CellAutomataAgent::R_MATE, 1),
  _iAgent(owner)
{
  // Nothing;
}


void CellAutomataAgent::PGrouping::onMemberDisconnected(const k_guid_t& guid) {
  // Nothing;
}


void CellAutomataAgent::PGrouping::onAllMembersConnected() {
  if(!_iAgent->_pDisplayInfo.queryLocalDisplays()) {
    LOG_ERR << "Local display setup query failed. Quitting." << EL;
    _iAgent->quit();
  } else if(_iAgent->getRuntime().isHead()){
    if(_iAgent->_pDisplayInfo.queryPeerDisplays()) {
      _iAgent->makeAndDistributePartitionTable();
    }
  }
}


//\/ CellAutomataAgent::ComputeThread /\///////////////////////////////////////

CellAutomataAgent::ComputeThread::ComputeThread(CellAutomataAgent& agent)
: Thread(agent.getAlias() + " compute thread"),
  _iAgent(agent)
{
  // Nothing;
}


void CellAutomataAgent::ComputeThread::run() {
  _iAgent.setupLayers();
  _iAgent.computeLoop();
}


//\/ CellAutomataAgent /\//////////////////////////////////////////////////////

// --- STATIC FIELDS --- //

SPtr<KGridType> CellAutomataAgent::PARTITION_MAP_T;
SPtr<KGridType> CellAutomataAgent::CELL_GRID_T;
SPtr<KRecordType> CellAutomataAgent::BORDER_T;

const SPtr<KString> CellAutomataAgent::OP_START
    = KS("knorba.demo.cell-automata.start");

const SPtr<KString> CellAutomataAgent::OP_PARTITION
    = KS("knorba.demo.cell-automata.partition");

const SPtr<KString> CellAutomataAgent::OP_BORDER
    = KS("knorba.demo.cell-automata.border");

const SPtr<KString> CellAutomataAgent::OP_SET_DELAY
    = KS("knorba.demo.cell-automata.set-delay");

const SPtr<KString> CellAutomataAgent::R_MATE = KS("mate");
const SPtr<KString> CellAutomataAgent::R_OUTPUT = KS("output");


// --- STATIC METHODS --- //

SPtr<KGridType> CellAutomataAgent::partition_map_t() {
  if(PARTITION_MAP_T.isNull()) {
    PARTITION_MAP_T = new KGridType(Partition::type(), 1);
  }
  return PARTITION_MAP_T;
}


SPtr<KGridType> CellAutomataAgent::cell_grid_t() {
  if(CELL_GRID_T.isNull()) {
    CELL_GRID_T = new KGridType(KType::OCTET, 2);
  }
  return CELL_GRID_T;
}


SPtr<KRecordType> CellAutomataAgent::border_t() {
  if(BORDER_T.isNull()) {
    BORDER_T = new KRecordType("knorba.demo.cell-automata.Border");
    BORDER_T->addField("phase", KType::INTEGER)
            ->addField("ofset", KPoint::type().AS(KType))
            ->addField("data", cell_grid_t().AS(KType));
  }
  return BORDER_T;
}


Ptr< Array<int> > CellAutomataAgent::divideScalarRange(int begin, int end,
    int n)
{
  Ptr< Array<int> > divs = new Array<int>();
  divs->setSize(n + 1);
  
  int size = end - begin;
  int divSize = size/n;
  int remainder = size%n;
  
  divs->at(0) = 0;
  for(int i = 1; i <= n; i++) {
    divs->at(i) = divs->at(i - 1) + divSize;
    if(i <= remainder) {
      divs->at(i)++;
    }
  }

  return divs;
}


Ptr<CellAutomataAgent::Partition> CellAutomataAgent::nodeToPartition(
    PPtr<Array<int> > xDivs, PPtr<Array<int> > yDivs,
    PPtr<DisplayInfoProtocol::NodeInfo> node)
{
  Ptr<Partition> p = new Partition();
  
  const Tuple& begin = node->getIndexRange().getBegin();
  const Tuple& end = node->getIndexRange().getEnd();
  
  p->guid = node->getGuid();
  p->rect = KRect(
      Tuple2D(xDivs->at(begin.at(0)), yDivs->at(begin.at(1))),
      Tuple2D(xDivs->at(end.at(0))  , yDivs->at(end.at(1))));
  
  return p;
}


string CellAutomataAgent::toString(PPtr< Array<int> > a) {
  int s = a->getSize();
  string str = "{";
  for(int i = 0; i < s; i++) {
    if(i > 0) {
      str.append(", ");
    }
    str.append(Int::toString(a->at(i)));
  }
  str.append("}");
  return str;
}


// --- (DE)CONSTRUCTORS --- //

CellAutomataAgent::CellAutomataAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid),
  _pGrouping(this),
  _pDisplayInfo(this, R_MATE),
  _pConsole(this),
  _pPixmap(this, PixmapOutputProtocol::BLACK_AND_WHITE, R_OUTPUT),
  _borderCond(false)
{
  _delay = 200;
  _stopFlag = false;
  _pConsole.setServer(getRuntime().getConsoleGuid());
  _computeThread = new ComputeThread(*this);
  _neighbours = new ManagedArray<Partition>();
  registerHandler((handler_t)&CellAutomataAgent::handleOpStart, OP_START);
  registerHandler((handler_t)&CellAutomataAgent::handleOpPartition, OP_PARTITION);
  registerHandler((handler_t)&CellAutomataAgent::handleOpBorder, OP_BORDER);
  registerHandler((handler_t)&CellAutomataAgent::handleOpSetDelay, OP_SET_DELAY);
}


// --- METHODS --- //

void CellAutomataAgent::makeAndDistributePartitionTable() {
  Range indexRange = _pDisplayInfo.getGlobalIndexRange();

  Tuple nDivs = indexRange.getSize();
  
  Ptr< Array<int> > xDivs = divideScalarRange(0, _globalSize.at(0), nDivs.at(0));
  Ptr< Array<int> > yDivs = divideScalarRange(0, _globalSize.at(1), nDivs.at(1));
  
  PPtr< ManagedArray<DisplayInfoProtocol::NodeInfo> > nodes
      = _pDisplayInfo.getRemoteInfo();
  
  Ptr< ManagedArray<Partition> > partitions = new ManagedArray<Partition>();
  
  for(int i = nodes->getSize() - 1; i >= 0; i--) {
    partitions->push(nodeToPartition(xDivs, yDivs, nodes->at(i)));
  }
  
  _local = nodeToPartition(xDivs, yDivs, _pDisplayInfo.getLocalInfo());
  
  partitions->push(_local);
  
  ALOG << "Partition Map: " << *partitions << EL;
  
  Tuple1D gridSize(partitions->getSize());
  Ptr<KGridBasic> grid = new KGridBasic(partition_map_t(), gridSize);
  Ptr<KRecord> gridRecord = new KRecord(grid.AS(KGrid));
  for(RangeIterator i(gridSize); i.hasMore(); i.next()) {
    partitions->at(i.at(0))->toKRecord(grid->at(i, gridRecord));
  }
  
  send(R_MATE, OP_PARTITION, grid.AS(KValue));
  
  for(int i = partitions->getSize() - 1; i >= 0; i--) {
    if(partitions->at(i)->rect.isAdjecentTo(_local->rect)) {
      _neighbours->push(partitions->at(i));
    }
  }
  
  _computeThread->start();
}


void CellAutomataAgent::setupLayers() {
  Range localRect = _local->rect;
  Tuple memorySize = localRect.grow(1).getSize();
  _computeRange = Range(memorySize).shrink(1);
  _globalToLocal = _computeRange.getBegin() - _local->rect.getBegin();
  
  ALOG << "localRect: " << localRect << EL;
  ALOG << "computeRange: " << _computeRange << EL;
  
  _layer1 = new KGridBasic(cell_grid_t(), memorySize);
  _layer2 = new KGridBasic(cell_grid_t(), memorySize);
  
  Ptr<KRecord> rec1 = new KRecord(_layer1.AS(KGrid));
  Ptr<KRecord> rec2 = new KRecord(_layer2.AS(KGrid));
  
  // Randomizing layer1
  for(RangeIterator i(memorySize); i.hasMore(); i.next()) {
    if(_computeRange.contains(i)) {
      _layer1->at(i, rec1)->setOctet(rand()%2);
    } else {
      _layer1->at(i, rec1)->setOctet(0);
    }
    _layer2->at(i, rec2)->setOctet(0);
  }
  
  // Setup Neighbours
  for(int i = _neighbours->getSize() - 1; i >= 0; i--) {
    PPtr<Partition> p = _neighbours->at(i);
    Range border = _local->rect.intersectWith(p->rect.grow(1));
    Range borderOnLocal = border.translate(_globalToLocal);
    p->window = new KGridWindow(_layer1.AS(KGrid), borderOnLocal);
    p->toSend = new KRecord(border_t());
    p->toSend->setGrid(BORDER_T_DATA, p->window.AS(KGrid));
    p->phase = -1;
    KPoint(border.getBegin()).toKRecord(p->toSend->getRecord(BORDER_T_OFSET));
  }
}


void CellAutomataAgent::computeLoop() {
  _readFromLayer1 = true;
  
  Ptr<KRecord> rec1 = new KRecord(_layer1.AS(KGrid));
  Ptr<KRecord> rec2 = new KRecord(_layer2.AS(KGrid));
  PPtr<KGridBasic> readLayer = _layer1;
  PPtr<KGridBasic> writeLayer = _layer2;
  PPtr<KRecord> readRec = rec1;
  PPtr<KRecord> writeRec = rec2;

  Ptr<KGridWindow> displayWindow
      = new KGridWindow(_layer1.AS(KGrid), _computeRange);

  int phase = 0;
  ProximityIterator j(1);
  
  while(!_stopFlag) {
    displayWindow->setSource(readLayer.AS(KGrid));
    _pPixmap.tput(phase, displayWindow.AS(KGrid));
    
    exchangeBorders(phase);
    
    System::sleep(_delay);
    
    // Iterating over data
    for(RangeIterator i(_computeRange); i.hasMore(); i.next()) {
      int n = 0;
      for(j.centerAt(i); j.hasMore(); j.next()) {
        if(!j.equals(i)) {
          n += readLayer->at(j, readRec)->getOctet();
        }
      }
      
      // Evaluating Conway's game of life
      bool isLiving = readLayer->at(i, readRec)->getOctet();
      if(n < 2 && isLiving) {
        writeLayer->at(i, writeRec)->setOctet(0);
      } else if(isLiving && (n == 2 || n == 3)) {
        writeLayer->at(i, writeRec)->setOctet(1);
      } else if(isLiving && n > 3) {
        writeLayer->at(i, writeRec)->setOctet(0);
      } else if(!isLiving && n == 3) {
        writeLayer->at(i, writeRec)->setOctet(1);
      } else {
        writeLayer->at(i, writeRec)->setOctet(isLiving);
      }
    }
    
    
    _readFromLayer1 = !_readFromLayer1;
    
    if(_readFromLayer1) {
      readLayer = _layer1;
      writeLayer = _layer2;
      readRec = rec1;
      writeRec = rec2;
    } else {
      readLayer = _layer2;
      writeLayer = _layer1;
      readRec = rec2;
      writeRec = rec1;
    }
    
    phase++;
  }
}


void CellAutomataAgent::exchangeBorders(int phase) {
  if(_neighbours->getSize() == 0) {
    return;
  }
  
  for(int i = _neighbours->getSize() - 1; i >= 0; i--) {
    PPtr<Partition> p = _neighbours->at(i);
    if(_readFromLayer1) {
      p->window->setSource(_layer1.AS(KGrid));
    } else {
      p->window->setSource(_layer2.AS(KGrid));
    }
    p->toSend->setInteger(BORDER_T_PHASE, phase);
    send(p->guid, OP_BORDER, p->toSend.AS(KValue));
  }
  
  bool block = true;
  while(block && !_stopFlag) {
    block = false;
    for(int i = _neighbours->getSize() - 1; i >= 0; i--) {
      if(_neighbours->at(i)->phase < phase) {
        block = true;
      }
    }
    
    if(block && !_stopFlag) {
      _borderCond.block();
    }
  }
}


void CellAutomataAgent::dump(PPtr<KGrid> g, int phase) {
  Tuple2D point;
  Range r = g->getRange();
  int maxX = r.getEnd().at(0);
  int maxY = r.getEnd().at(1);
  
  string str = "Phase: " + Int::toString(phase) + "\n";
  
  Ptr<KRecord> rec = new KRecord(g);
  for(int y = r.getBegin().at(0); y < maxY; y++) {
    for(int x = r.getBegin().at(1); x < maxX; x++) {
      point.set(x, y);
      if(g->at(point, rec)->getOctet()) {
        str.push_back('O');
      } else {
        str.push_back('.');
      }
    }
    str.push_back('\n');
  }
  
  cout << str << endl;
}


// Handlers //

void CellAutomataAgent::handleOpStart(PPtr<Message> msg) {
  _globalSize = KPoint(msg->getPayload().AS(KRecord));
  _pGrouping.start();
}


void CellAutomataAgent::handleOpPartition(PPtr<Message> msg) {
  Ptr<KGrid> partitionMap = msg->getPayload().AS(KGrid);
  Ptr<KRecord> r = new KRecord(partitionMap);
  Ptr< ManagedArray<Partition> > allPartitions = new ManagedArray<Partition>();
  for(RangeIterator i(partitionMap->getRange()); i.hasMore(); i.next()) {
    Ptr<Partition> p = new Partition(partitionMap->at(i, r));
    if(p->guid == getGuid()) {
      _local = p;
    } else {
      allPartitions->push(p);
    }
  }
  
  if(_local.isNull()) {
    LOG_ERR << "No partition for this node" << EL;
    return;
  }
  
  for(int i = allPartitions->getSize() - 1; i >= 0; i--) {
    if(allPartitions->at(i)->rect.isAdjecentTo(_local->rect)) {
      _neighbours->push(allPartitions->at(i));
    }
  }
  
  _computeThread->start();
}


void CellAutomataAgent::handleOpBorder(PPtr<Message> msg) {
  const k_guid_t& sender = msg->getSender();
  
  for(int i = _neighbours->getSize() - 1; i >= 0; i--) {
    PPtr<Partition> p = _neighbours->at(i);
    if(p->guid == sender) {
      PPtr<KRecord> r = msg->getPayload().AS(KRecord);
      p->phase = r->getInteger(BORDER_T_PHASE);
      KPoint offset(r->getRecord(BORDER_T_OFSET));
      PPtr<KGrid> data = r->getGrid(BORDER_T_DATA);
      PPtr<KGridBasic> target = _readFromLayer1?_layer1:_layer2;
      
      target->copyFrom(data.AS(KGrid), Tuple2D::ZERO, offset + _globalToLocal,
          data->getRange().getSize());
    }
  }
  
  _borderCond.release();
}


void CellAutomataAgent::handleOpSetDelay(PPtr<Message> msg) {
  _delay = msg->getPayload().AS(KInteger)->get();
}


// Inherited from Agent //

void CellAutomataAgent::handlePeerConnectionRequest(PPtr<KString> role,
    const k_guid_t& guid)
{
  if(role->equals(R_OUTPUT)) {
    addPeer(R_OUTPUT, guid);
  }
}


bool CellAutomataAgent::isAlive() {
  return Agent::isAlive() || _computeThread->isRunning();
}


void CellAutomataAgent::finalize() {
  _stopFlag = true;
  _borderCond.release();
  Agent::finalize();
}