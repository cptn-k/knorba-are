//
//  RunElement.h
//  ARE
//
//  Created by Hamed KHANDAN on 9/17/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef __ARE__RunCommand__
#define __ARE__RunCommand__

// KFoundation
#include <kfoundation/ManagedObject.h>
#include <kfoundation/SerializingStreamer.h>
#include <kfoundation/StreamDeserializer.h>
#include <kfoundation/ManagedArrayDecl.h>

namespace kfoundation {
  class Token;
  class ObjectToken;
}

namespace knorba {
  class Runtime;
}

namespace knorba {
namespace are {
  
  using namespace kfoundation;
  
  class MessageElement;
  class PeerElement;

  class RunElement :
      public ManagedObject,
      public SerializingStreamer,
      public StreamDeserializer
  {
    
  // --- STATIC FIELDS --- //
    
    public: static const string RUN;
    public: static const string CLASS;
    public: static const string ALIAS;
    
    
  // --- FIELDS --- //
    
    private: string _alias;
    private: string _class;
    private: bool _hasAlias;
    
    
  // --- (DE)CONSTRUCTORS --- //
    
    public: RunElement();
    
    
  // --- METHODS --- //
    
    public: void read(PPtr<ObjectToken> headToken);
    public: bool hasAlias() const;
    public: const string& getAlias() const ;
    public: const string& getClassName() const;
    
    // Inherited from StreamDeserializer //
    public: void deserialize(PPtr<ObjectToken> headToken);
    
    // Inherited from SerializingStreamer //
    public: void serialize(PPtr<ObjectSerializer> serializer) const;
    
  };
  
} // are
} // knorba

#endif /* defined(__ARE__RunCommand__) */
