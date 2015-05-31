//
//  UnixSocketOutputStream.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 4/6/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#define MAX_QUEUE_SIZE 2
#define SOCK_CAPACITY 4096

#include "UnixSocketOutputStream.h"

// Unix
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/Path.h>

namespace knorba {
namespace are {
  
  UnixSocketOutputStream::UnixSocketOutputStream() {
    _isOpen = false;
  }
  
  
  UnixSocketOutputStream::~UnixSocketOutputStream() {
    if(_isOpen) {
      close();
    }
  }
  
  
  void UnixSocketOutputStream::connect(PPtr<Path> path) throw(IOException) {
    _addr.sun_family = AF_UNIX;
    strncpy(_addr.sun_path, path->getString().c_str(), sizeof(_addr.sun_path));
    
    _socket = socket(AF_UNIX, SOCK_STREAM, 0);
    int err = ::connect(_socket, (sockaddr*)&_addr, sizeof(sockaddr));
    
    if(err != 0) {
      throw IOException("Could not connect to " + string(_addr.sun_path)
                        + ". Reason: " + System::getLastSystemError());
    }
    
    _isOpen = true;
  }
  
  
  bool UnixSocketOutputStream::isOpen() {
    return _isOpen;
  }
  
  
  bool UnixSocketOutputStream::isBigEndian() const {
    return System::isBigEndian();
  }
  
  
  void UnixSocketOutputStream::write(const kf_octet_t* buffer,
      const kf_int32_t nBytes)
  {
    if(!_isOpen) {
      throw IOException("Attemp to write to a closed socket.");
    }
    
    ssize_t octetsLeftToSend = nBytes;
    ssize_t totalSent = 0;
    
    while(octetsLeftToSend > 0) {
      ssize_t octetsToSend
      = (octetsLeftToSend <= SOCK_CAPACITY)?octetsLeftToSend:SOCK_CAPACITY;
      
      ssize_t s = ::write(_socket, buffer + totalSent, octetsToSend);
      
      if(s < 0) {
        throw IOException("Failed to write to output. Reason: "
                          + System::getLastSystemError());
      }
      
      octetsLeftToSend -= s;
      totalSent += s;
    }
  }
  
  
  void UnixSocketOutputStream::write(kf_octet_t byte) {
    ssize_t s = ::write(_socket, &byte, 1);
    if(s < 0) {
      throw IOException("Failed to write to output. Reason: "
                        + System::getLastSystemError());
    }
  }
  
  
  void UnixSocketOutputStream::write(PPtr<InputStream> os) {
    throw KFException("Operation not supported.");
  }
  
  
  void UnixSocketOutputStream::close() {
    ::close(_socket);
    _isOpen = false;
  }

  
} // namespace are
} // namespace knorba