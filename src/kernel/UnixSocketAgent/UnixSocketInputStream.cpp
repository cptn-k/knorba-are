//
//  UnixSocketInputStream.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 4/6/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#include "UnixSocketInputStream.h"

// Unix
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/Path.h>

#define MAX_QUEUE_SIZE 2
#define SOCK_CAPACITY 4096


namespace knorba {
namespace are {

  UnixSocketInputStream::UnixSocketInputStream() {
    _isBound = false;
    _isOpen = false;
  }
  
  
  UnixSocketInputStream::~UnixSocketInputStream() {
    if(isOpen()) {
      close();
    }
    
    if(isBound()) {
      unbind();
    }
  }
  
  
  void UnixSocketInputStream::bind(PPtr<Path> path) throw(IOException) {
    _path = path;
    
    if(path->exists()) {
      path->remove();
    }
    
    _addr.sun_family = AF_UNIX;
    strncpy(_addr.sun_path, path->getString().c_str(), sizeof(_addr.sun_path));
    
    _hostSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    int err = ::bind(_hostSocket, (sockaddr*)&_addr, sizeof(_addr));
    
    if(err != 0) {
      throw IOException("Could not bind to host " + *path + ". Reason: "
                        + System::getLastSystemError());
    }
    
    _isBound = true;
  }
  
  
  void UnixSocketInputStream::unbind() {
    if(!_isBound) {
      return;
    }
    
    if(isOpen()) {
      close();
    }
    
    _isBound = false;
    
    shutdown(_hostSocket, SHUT_RDWR);
    ::close(_hostSocket);

    _path->remove();
    _path = NULL;
    
  }
  
  
  bool UnixSocketInputStream::isBound() const {
    return _isBound;
  }
  
  
  void UnixSocketInputStream::listen() throw(IOException) {
    if(!_isBound) {
      throw IOException("Cannot initiate listening. Socket is not bound.");
    }
    
    if(_isOpen) {
      throw IOException("Cannot initiate listentin. Socket is in use.");
    }
    
    int err = ::listen(_hostSocket, MAX_QUEUE_SIZE);
    if(err != 0){
      throw IOException("Could not initiate listening as "
                        + string(_addr.sun_path) + ". Reason: " + System::getLastSystemError());
    }
    
    sockaddr_un senderAddress;
    socklen_t len = sizeof(sockaddr_un);
    
    _readSocket = accept(_hostSocket, (sockaddr*)&senderAddress, &len);
    
    if(!_isBound) {
      return;
    }
    
    if(_readSocket < 0) {
      throw IOException("Failed to accept connection. Reason: "
                        + System::getLastSystemError());
    }
    
    _isOpen = true;
  }
  
  
  bool UnixSocketInputStream::isOpen() const {
    return _isOpen;
  }
  
  
  void UnixSocketInputStream::close() {
    ::close(_readSocket);
    _isOpen = false;
  }
  
  
  PPtr<Path> UnixSocketInputStream::getPath() const {
    return _path;
  }
  
  
  kf_int32_t UnixSocketInputStream::read(kf_octet_t* buffer, const kf_int32_t nBytes) {
    if(!_isOpen) {
      throw IOException("Attemp to read a closed socket.");
    }
    
    kf_int32_t totalRead = 0;
    
    while(totalRead < nBytes) {
      kf_int32_t s = (kf_int32_t)::read(_readSocket, buffer + totalRead, nBytes - totalRead);
      
      if(s == 0) {
        _isEof = true;
        break;
      }
      
      totalRead += s;
    }
    
    return totalRead;
  }
  
  
  int UnixSocketInputStream::read() {
    kf_octet_t v;
    size_t s = ::read(_readSocket, &v, 1);
    
    if(s == 0) {
      _isEof = true;
      return -1;
    }
    
    return v;
  }
  
  
  int UnixSocketInputStream::peek() {
    throw KFException("peek() is not supported.");
  }
  
  
  kf_int32_t UnixSocketInputStream::skip(kf_int32_t bytes) {
    throw KFException("skip() is not supported.");
  }
  
  
  bool UnixSocketInputStream::isEof() {
    return _isEof;
  }
  
  
  bool UnixSocketInputStream::isMarkSupported() {
    return false;
  }
  
  
  void UnixSocketInputStream::mark() {
    throw KFException("mark() is not supported.");
  }
  
  
  void UnixSocketInputStream::reset() {
    throw KFException("reset() is not supported.");
  }
  
  
  bool UnixSocketInputStream::isBigEndian() {
    return System::isBigEndian();
  }

  
} // namespace are
} // namespace knorba
