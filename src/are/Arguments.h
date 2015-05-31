//
//  Arguments.h
//  KnoRBA-ARE-XCodeWrapper
//
//  Created by Hamed KHANDAN on 8/27/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef KNORBA_ARE_ARGUMENTS
#define KNORBA_ARE_ARGUMENTS

#include <kfoundation/ManagedObject.h>
#include <kfoundation/Streamer.h>

using namespace std;
using namespace kfoundation;

namespace knorba {
namespace are {
  
  class Arguments : public ManagedObject, public Streamer {
    
  // --- NESTED TYPES --- //
    
    public: typedef enum {
      RUN,
      START,
      STOP,
      RESTART,
      STATUS,
      KILL,
      ERROR
    } verb_t;
    
    
  // --- FIELDS --- //
    
    private: int    _argc;
    private: char** _argv;
    private: verb_t _verb;
    private: string _bundleName;
    private: string _error;
    private: int    _killIndex;

    
  // --- STATIC METHODS --- //
    
    public: static void printUsuage(ostream& os);
    
  
  // --- CONSTRUCTOR --- //
    
    public: Arguments(int argc, char** argv);
    
    
  // --- METHODS --- //
    
    private: int readVerb(int index, verb_t v, const string& str);
    private: int readBundleName(int index);
    private: int readKillIndex(int index);
    
    public: verb_t getVerb() const;
    public: const string& getBundleName() const;
    public: int getKillIndex() const;
    public: const string& getError() const;
    
    public: void printToStream(ostream& os) const;
    
  };
  
} // are
} // knorba



#endif /* defined(KNORBA_ARE_ARGUMENTS) */
