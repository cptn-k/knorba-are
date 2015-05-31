//
//  CellAutomataAgent.h
//  AgentRuntimeEnvironment
//
//  Created by Kay Khandan on 2/28/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__CellAutomataAgent__
#define __AgentRuntimeEnvironment__CellAutomataAgent__

using namespace knorba;
using namespace knorba::type;
using namespace knorba::protocol;

//\/ GLOBAL /\/////////////////////////////////////////////////////////////////

extern "C" void init(Runtime& rt);
extern "C" knorba::Agent* instantiate(Runtime& rt, const k_guid_t& guid);


//\/ KPoint /\/////////////////////////////////////////////////////////////////

class KPoint : public Tuple2D {
  private: static SPtr<KRecordType> TYPE;
  private: static const k_octet_t X = 0;
  private: static const k_octet_t Y = 1;
  public: static SPtr<KRecordType> type();
  public: KPoint();
  public: KPoint(const Tuple& other);
  public: KPoint(int x, int y);
  public: KPoint(PPtr<KRecord> r);
  public: void toKRecord(PPtr<KRecord> r) const;
};


//\/ KRect /\//////////////////////////////////////////////////////////////////

class KRect : public Range {
  private: static SPtr<KRecordType> TYPE;
  private: static const k_octet_t BEGIN = 0;
  private: static const k_octet_t END = 1;
  public: static SPtr<KRecordType> type();
  public: KRect();
  public: KRect(const Range& other);
  public: KRect(const Tuple& begin, const Tuple& end);
  public: KRect(PPtr<KRecord> r);
  public: void toKRecord(PPtr<KRecord> r) const;
};


//\/ CellAutomataAgent /\//////////////////////////////////////////////////////

class CellAutomataAgent : public Agent {
  
// --- NESTED TYPES --- //
  
  private: typedef enum {
    LIFE = 0,
    SMOOTH_LIFE = 1,
    ROCK_PAPER_SCISORS = 3
  } problem_t;

  
  public: class Partition : public ManagedObject, public SerializingStreamer {
    private: static SPtr<KRecordType> TYPE;
    private: static const k_octet_t GUID = 0;
    private: static const k_octet_t RECT = 1;
    
    public: static SPtr<KRecordType> type();
    
    public: Ptr<KGridWindow> window;
    public: Ptr<KRecord> toSend;
    public: KRect rect;
    public: k_guid_t guid;
    public: k_integer_t phase;
    
    public: Partition();
    public: Partition(PPtr<KRecord> r);
    public: void toKRecord(PPtr<KRecord> r) const;
    public: void serialize(PPtr<ObjectSerializer> os) const;
  };
  
  
  private: class PGrouping : public GroupingProtocol {
    private: CellAutomataAgent* _iAgent;
    public: PGrouping(CellAutomataAgent* owner);
    public: void onMemberDisconnected(const k_guid_t& guid);
    public: void onAllMembersConnected();
  };
  
  
  private: class ComputeThread : public Thread {
    private: CellAutomataAgent& _iAgent;
    public: ComputeThread(CellAutomataAgent& agent);
    public: void run();
  };
  
  
// --- STATIC FIELDS --- //
  
  private: static SPtr<KGridType> PARTITION_MAP_T;
  private: static SPtr<KGridType> CELL_GRID_T;
  private: static SPtr<KRecordType> BORDER_T;
  public: static const k_octet_t BORDER_T_PHASE = 0;
  public: static const k_octet_t BORDER_T_OFSET = 1;
  public: static const k_octet_t BORDER_T_DATA = 2;
  public: static const SPtr<KString> OP_START;
  public: static const SPtr<KString> OP_PARTITION;
  public: static const SPtr<KString> OP_BORDER;
  public: static const SPtr<KString> OP_SET_DELAY;
  public: static const SPtr<KString> R_MATE;
  public: static const SPtr<KString> R_OUTPUT;
  
  
// --- STATIC METHODS --- //
  
  public: static SPtr<KGridType> partition_map_t();
  public: static SPtr<KGridType> cell_grid_t();
  public: static SPtr<KRecordType> border_t();
  public: static Ptr< Array<int> > divideScalarRange(int begin, int end, int n);
  public: static Ptr<Partition> nodeToPartition(PPtr< Array<int> > xDivs,
      PPtr< Array<int> > yDivs, PPtr<DisplayInfoProtocol::NodeInfo> node);
  public: static string toString(PPtr< Array<int> > a);

// --- FIELDS --- //
  
  private: Ptr<ComputeThread> _computeThread;
  private: Ptr< ManagedArray<Partition> > _neighbours;
  private: Ptr<Partition> _local;
  private: Ptr<KGridBasic> _layer1;
  private: Ptr<KGridBasic> _layer2;
  private: DisplayInfoProtocol _pDisplayInfo;
  private: ConsoleProtocolClient _pConsole;
  private: PixmapOutputProtocol _pPixmap;
  private: Condition _borderCond;
  private: Tuple _globalToLocal;
  private: Range _computeRange;
  private: PGrouping _pGrouping;
  private: KPoint _globalSize;
  private: int _delay;
  private: bool _readFromLayer1;
  private: bool _stopFlag;
  
  
// --- (DE)CONSTRUCTORS --- //
  
  public: CellAutomataAgent(Runtime& rt, const k_guid_t& guid);
  
  
// --- METHODS --- //
  
  private: void makeAndDistributePartitionTable();
  private: void setupLayers();
  private: void computeLoop();
  private: void exchangeBorders(int phase);
  private: void dump(PPtr<KGrid> g, int phase);
  
  // Handlers //
  public: void handleOpStart(PPtr<Message> msg);
  public: void handleOpPartition(PPtr<Message> msg);
  public: void handleOpBorder(PPtr<Message> msg);
  public: void handleOpSetDelay(PPtr<Message> msg);
  
  // Inherited from Agent
  public: void handlePeerConnectionRequest(PPtr<KString> role, const k_guid_t &guid);
  public: bool isAlive();
  public: void finalize();
  
};

#endif /* defined(__AgentRuntimeEnvironment__CellAutomataAgent__) */