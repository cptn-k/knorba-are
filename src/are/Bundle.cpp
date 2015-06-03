//
//  Manifesto.cpp
//  KnoRBA-ARE-XCodeWrapper
//
//  Created by Hamed KHANDAN on 9/1/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

// Std
#include <cstdlib>

// KFoundation
#include <kfoundation/Logger.h>
#include <kfoundation/FileInputStream.h>
#include <kfoundation/FileOutputStream.h>
#include <kfoundation/BufferOutputStream.h>
#include <kfoundation/XmlObjectStreamReader.h>
#include <kfoundation/File.h>
#include <kfoundation/Path.h>

// KnoRBA
#include <knorba/type/KGlobalUid.h>

// Internal
#include "AreException.h"

// Self
#include "Bundle.h"

namespace knorba {
namespace are {
  
  using namespace std;
  
//\/ Bundle /\/////////////////////////////////////////////////////////////////
  
// --- STATIC FIELDS --- //
  
  k_integer_t Bundle::SIZE_OF_MD5_IN_OCTETS = 16;
  
  
// --- (DE)CONSTRUCTOR --- //
    
  Bundle::Bundle() {
    // Nothing
  }
  
  Bundle::Bundle(PPtr<Path> path, PPtr<Path> knorbaHome)
  {
    _src = path;
    _knorbaHome = knorbaHome;
    
    if(path->hasExtention()) {
      if(path->getExtention() == "kap") {
        _srcIsKap = true;
      } else if(path->getExtention() == "kar") {
        _srcIsKap = false;
      } else {
        throw AreException("Brovided bundle name does not have recognized "
            "extension: " + path->getFileNameWithExtension());
      }
    } else {
      _src = path->changeExtension("kap");
      if(_src->exists()) {
        _srcIsKap = true;
      } else {
        _src = path->changeExtension("kar");
        if(_src->exists()) {
          _srcIsKap = false;
        } else {
          throw AreException("Could not locate bundle \""
              + path->getString() + "\"");
        }
      }
    }

    Ptr<Path> archiveDir = _knorbaHome->addSegement("repo");
    
    if(!archiveDir->exists()) {
      archiveDir->makeDir();
    }
    
    _appName = _src->getFileName();
    
    _kar = archiveDir->addSegement(_appName + ".kar");
    _kap = archiveDir->addSegement(_appName + ".kap");
  }
  
  
  Bundle::~Bundle() {
    // Nothing
  }
  
  
// --- METHODS --- //

  void Bundle::readMd5(PPtr<Path> file, k_octet_t *md5) {
    Ptr<FileInputStream> fis = new FileInputStream(file);
    for(int i = 0; i < 16; i++) {
      int byte = fis->read();
      if(byte == -1) {
        throw AreException("Not enough bytes to read in md5 file: "
            + file->getString());
      }
      md5[i] = byte;
    }
    fis->close();
  }
  
  
  void Bundle::writeMd5(PPtr<Path> file, const k_octet_t *md5) {
    Ptr<FileOutputStream> fos = new FileOutputStream(file);
    fos->truncate();
    fos->write((kf_octet_t*)md5, 16);
    fos->close();
  }
  
  
  void Bundle::compress(PPtr<Path> src, PPtr<Path> target) {
    Ptr<Path> archivePath = target->parent();
    
    if(!archivePath->exists()) {
      archivePath->makeDir();
    }
    
    string tarCommand = _knorbaHome->getString() + "/bin/tar -C"
        + src->parent()->getString() + " -cf " + target->getString() + " "
        + src->getFileNameWithExtension();
    
    LOG << "Creating: " << target->getFileNameWithExtension() << EL;
    LOG << tarCommand << EL;
    
    #ifdef KF_UNIX
    if(system(tarCommand.c_str()) == -1) {
      throw AreException("Error creating archive with command: " + tarCommand);
    }
    #else
    #  error "Platform not supported."
    #endif
  }
  
  
  void Bundle::decompress(PPtr<Path> src, PPtr<Path> target) {
    string tarCommand = _knorbaHome->getString() + "/bin/tar -C "
        + target->getString() + " -xf " + src->getString();
    
    LOG << "Repositing: " << src->getFileName() << ".kap" << EL;
    LOG << tarCommand << EL;
    
    #ifdef KF_UNIX
    if(system(tarCommand.c_str()) == -1) {
      throw AreException("Error extracting archive with command: " + tarCommand);
    }
    #else
    #  error "Platform not supported."
    #endif
  }
  
  
  void Bundle::copy(PPtr<Path> src, PPtr<Path> dst) {
    #ifdef KF_UNIX
    string copyCommand = "cp -Rf \"" + src->getString() + "\" \""
                       + dst->getString() + "\"";
    
    if(system(copyCommand.c_str()) == -1) {
      throw AreException("Error executing copy command: " + copyCommand);
    }
    #else
    #  error "Platform not supported."
    #endif
  }
  
  
  void Bundle::rm(PPtr<Path> path) {
    #ifdef KF_UNIX
    string command = "rm -rf \"" + path->getString() + "\"";
    
    if(system(command.c_str()) == -1) {
      throw AreException("Error executing rm command: " + command);
    }
    #else
    #  error "Platform not supported."
    #endif
  }
  
  
  bool Bundle::checkMd5(const k_octet_t *md5) {
    Ptr<Path> md5File = _kar->changeExtension("md5");
    if(!md5File->exists()) {
      return false;
    }
    
    kf_octet_t existingMd5[SIZE_OF_MD5_IN_OCTETS];
    readMd5(md5File, existingMd5);
    if(memcmp(md5, existingMd5, SIZE_OF_MD5_IN_OCTETS) == 0) {
      return true;
    }
    
    return false;
  }
  
  
  void Bundle::pack(PPtr<InputStream> karData) {
    if(_kar->exists()) {
      _kar->remove();
    }
    
    Ptr<FileOutputStream> fos = new FileOutputStream(_kar);
    fos->write(karData);
    fos->close();
    
    _srcIsKap = false;
    
    pack();
  }
  
  
  void Bundle::pack() {
    Ptr<Path> md5File = _kar->changeExtension("md5");
    
    if(_srcIsKap) {
      compress(_src, _kar);
    } else {
      if(_src->getString() != _kar->getString()) {
        copy(_src, _kar);
      }
    }
    
    File::getMd5(_kar, _md5);
    
    bool changed = false;
    
    if(md5File->exists()) {
      k_octet_t prvMd5[16];
      readMd5(md5File, prvMd5);
      if(memcmp(_md5, prvMd5, 16) != 0) {
        changed = true;
        LOG << "Bundle is changed: " << _src->getFileName() << EL;
        LOG << BufferOutputStream::toBinaryString(prvMd5, 16) << EL;
        LOG << BufferOutputStream::toBinaryString(_md5, 16) << EL;
        rm(md5File);
        writeMd5(md5File, _md5);
        
        rm(_kap);
        decompress(_kar, _kap->parent());
      }
    } else {
      writeMd5(md5File, _md5);
      decompress(_kar, _kap->parent());
    }
    
    _manifestoPath = _kap->addSegement("manifesto.xml");    
  }

  
  PPtr<Path> Bundle::getKapPath() const {
    return _kap;
  }
  
  
  PPtr<Path> Bundle::getKarPath() const {
    return _kar;
  }
  
  
  string Bundle::getName() const {
    return _src->getFileName();
  }
  
  
  const k_octet_t* Bundle::getMd5() const {
    return _md5;
  }
  
  
  PPtr<Path> Bundle::getManifestoPath() const {
    return _manifestoPath;
  }
  
  
  const string& Bundle::getAppName() const {
    return _appName;
  }
  
  
  void Bundle::serialize(PPtr<ObjectSerializer> serializer) const {
    serializer->object("Bundle")
        ->attribute("srcIsKap", _srcIsKap)
        ->member("src")->object<Path>(_src)
        ->member("kap")->object<Path>(_kap)
        ->member("kar")->object<Path>(_kar)
        ->endObject();
  }
  
} // namespace are
} // namespace knorba
