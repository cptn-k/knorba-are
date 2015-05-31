//
//  UnixSocketInputStream.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 4/6/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__UnixSocketInputStream__
#define __AgentRuntimeEnvironment__UnixSocketInputStream__

// Unix
#include <sys/un.h>

// KFoundation
#include <kfoundation/IOException.h>

// Super
#include <kfoundation/InputStream.h>

namespace kfoundation {
  class Path;
}

namespace knorba {
namespace are {
  
  using namespace kfoundation;

  class UnixSocketInputStream : public InputStream {
    
  // --- FIELDS --- //
    
    private: Ptr<Path> _path;
    private: sockaddr_un _addr;
    private: int _hostSocket;
    private: int _readSocket;
    private: bool _isBound;
    private: bool _isOpen;
    private: bool _isEof;
    
    
  // --- (DE)CONSTRUCTORS --- //
    
    public: UnixSocketInputStream();
    public: ~UnixSocketInputStream();
    
  
  // --- METHODS --- //
    
    public: void bind(PPtr<Path> path) throw(IOException);
    public: void unbind();
    public: bool isBound() const;
    public: void listen() throw(IOException);
    public: bool isOpen() const;
    public: void close();
    public: PPtr<Path> getPath() const;
    
    // Inherited from InputStream //
    public: kf_int32_t read(kf_octet_t* buffer, const kf_int32_t nBytes);
    public: int read();
    public: int peek();
    public: kf_int32_t skip(kf_int32_t bytes);
    public: bool isEof();
    public: bool isMarkSupported();
    public: void mark();
    public: void reset();
    public: bool isBigEndian();
    
  };
  
} // namespace are
} // namespace knorba

#endif /* defined(__AgentRuntimeEnvironment__UnixSocketInputStream__) */