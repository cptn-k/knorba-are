//
//  UnixSocketOutputStream.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 4/6/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__UnixSocketOutputStream__
#define __AgentRuntimeEnvironment__UnixSocketOutputStream__

// Unix
#include <sys/un.h>

// KFoundation
#include <kfoundation/IOException.h>

// Super
#include <kfoundation/OutputStream.h>

namespace kfoundation {
  class Path;
}

namespace knorba {
namespace are {
  
  using namespace kfoundation;
  
  class UnixSocketOutputStream : public OutputStream {
    
  // --- FIELDS --- //
    
    private: sockaddr_un _addr;
    private: int _socket;
    private: bool _isOpen;
    
    
  // --- (DE)CONSTRUCTORS --- //
    
    public: UnixSocketOutputStream();
    public: ~UnixSocketOutputStream();
    
    
  // --- METHODS --- //
    
    public: void connect(PPtr<Path> target) throw(IOException);
    public: bool isOpen();
    
    // Inherited from OutputStream //
    public: bool isBigEndian() const;
    public: void write(const kf_octet_t* buffer, const kf_int32_t nBytes);
    public: void write(kf_octet_t byte);
    public: void write(PPtr<InputStream> os);
    public: void close();
  };
  
} // namespace are
} // namespace knorba

#endif /* defined(__AgentRuntimeEnvironment__UnixSocketOutputStream__) */
