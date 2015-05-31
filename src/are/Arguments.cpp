//
//  Arguments.cpp
//  KnoRBA-ARE-XCodeWrapper
//
//  Created by Hamed KHANDAN on 8/27/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#include <ostream>
#include <string>

#include <kfoundation/Int.h>

#include "Arguments.h"

namespace knorba {
namespace are {
  
  using namespace std;
  

// --- STATIC METHODS --- //
  
  void Arguments::printUsuage(ostream& os) {
    os << "Usage:\n"
       << "  are run <bundle-name> \n"
       << "  are <start|stop|restart|status>\n"
       << "  are kill [<index>|all]"
       << endl;
  }
  
  
// --- CONSTRUCTOR --- //
  
  Arguments::Arguments(int argc, char** argv)
  : _argc(argc),
    _argv(argv)
  {
    _verb = ERROR;
    
    if(argc <= 1) {
      _error = "No verb is specified.";
      return;
    }
    
    if(readVerb(1, RUN, "run") > 0) {
      readBundleName(2);
      return;
    }
    
    if(readVerb(1, START, "start") > 0) {
      return;
    }
    
    if(readVerb(1, STOP, "stop") > 0) {
      return;
    }
    
    if(readVerb(1, RESTART, "restart") > 0) {
      return;
    }
    
    if(readVerb(1, KILL, "kill") > 0) {
      readKillIndex(2);
      return;
    }
    
    if(readVerb(1, STATUS, "status") > 0) {
      return;
    }
    
    _error = "Unrecognized verb \"" + string(argv[1]) + "\".";
  }
  
  
// --- METHODS --- //

  int Arguments::readVerb(int index, verb_t v, const string &str) {
    if(index >= _argc || index < 0) {
      return -1;
    }
    
    if(string(_argv[index]) == str) {
      _verb = v;
      return index + 1;
    }
    
    return -1;
  }
  
  
  int Arguments::readBundleName(int index) {
    if(index >= _argc || index < 0) {
      _error = "No bundle name is specified.";
      _verb = ERROR;
      return -1;
    }
    
    _bundleName = string(_argv[index]);
    
    return index + 1;
  }
  
  
  int Arguments::readKillIndex(int index) {
    if(index >= _argc || index < 0) {
      _error = "No app is specified to kill.";
      _verb = ERROR;
      return -1;
    }
    
    string str(_argv[index]);
    if(str == "all") {
      _killIndex = -1;
    } else {
      _killIndex = Int::parse(string(_argv[index]));
    }
    
    return index + 1;
  }
  
  
  Arguments::verb_t Arguments::getVerb() const {
    return _verb;
  }
  
  
  const string& Arguments::getBundleName() const {
    return _bundleName;
  }
  
  
  int Arguments::getKillIndex() const {
    return _killIndex;
  }
  
  
  const string& Arguments::getError() const {
    return _error;
  }
  
  
  void Arguments::printToStream(ostream& os) const {
    switch (_verb) {
      case RUN:
        os << "run " << _bundleName;
        break;
        
      case START:
        os << "start";
        break;
        
      case STOP:
        os << "stop";
        break;
        
      case RESTART:
        os << "restart";
        break;
        
      case KILL:
        os << "kill ";
        if(_killIndex == -1) {
          os << "all";
        } else {
          os << _killIndex;
        }
        break;
        
        
      case STATUS:
        os << "status";
        break;
        
      case ERROR:
        os << "error!";
    }
  }
    
} // are
} // knorba
