//
//  Manifesto.h
//  KnoRBA-ARE-XCodeWrapper
//
//  Created by Hamed KHANDAN on 9/1/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef KNORBA_ARE_MANIFESTO
#define KNORBA_ARE_MANIFESTO


namespace knorba {
namespace are {
  
  using namespace kfoundation;
  using namespace knorba::type;
  
  class Bundle : public ManagedObject, public SerializingStreamer {
    
  // --- STATIC FIELDS --- //
    
    public: static k_integer_t SIZE_OF_MD5_IN_OCTETS;
    
    
  // --- FIELDS --- //
    
    private: bool      _srcIsKap;
    private: Ptr<Path> _knorbaHome;
    private: Ptr<Path> _src;
    private: Ptr<Path> _kap;
    private: Ptr<Path> _kar;
    private: Ptr<Path> _manifestoPath;
    private: string    _appName;
    private: string    _compileCommand;
    private: k_octet_t _md5[16];
    
    
  // --- (DE)CONSTRUCTOR --- //
    
    public: Bundle();
    public: Bundle(PPtr<Path> path, PPtr<Path> knorbaHome);
    public: ~Bundle();
    
    
  // --- METHODS --- //
    
    private: static void readMd5(PPtr<Path> file, k_octet_t md5[16]);
    private: static void writeMd5(PPtr<Path> file, const k_octet_t md5[16]);
    private: void compress(PPtr<Path> src, PPtr<Path> dst);
    private: void decompress(PPtr<Path> src, PPtr<Path> dst);
    private: void copy(PPtr<Path> src, PPtr<Path> dst);
    private: void rm(PPtr<Path> path);
    
    public:  bool checkMd5(const k_octet_t* md5);
    public:  void pack(PPtr<InputStream> karData);
    public:  void pack();
    public:  PPtr<Path> getKapPath() const;
    public:  PPtr<Path> getKarPath() const;
    public:  string getName() const;
    public:  const k_octet_t* getMd5() const;
    public:  PPtr<Path> getManifestoPath() const;
    public: const string& getAppName() const;
    
    // Inherited from SerializingStreamer
    public: void serialize(PPtr<ObjectSerializer> serializer) const;
    
  };
  
} // namespace are
} // namespace knorba

#endif /* defined(KNORBA_ARE_MANIFESTO) */
