//
//  PeerElement.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/31/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__PeerElement__
#define __AgentRuntimeEnvironment__PeerElement__

// KFoundation
#include <kfoundation/ManagedObject.h>
#include <kfoundation/SerializingStreamer.h>
#include <kfoundation/StreamDeserializer.h>

namespace knorba {
namespace are {
  
  using namespace kfoundation;
  
  class PeerElement : public ManagedObject, public SerializingStreamer,
      public StreamDeserializer
  {
    
  // --- STATIC FIELDS --- //
    
    public: static const string CONNECT;
    public: static const string SOURCE;
    public: static const string TARGET;
    public: static const string ROLE;
    
    
  // --- FIELDS --- //
    
    private: string _role;
    private: string _source;
    private: string _target;
    
    
  // --- (DE)CONSTRUCTORS --- //
    
    public: PeerElement();
    
    
  // --- METHODS --- //
    
    public: const string& getRole() const;
    public: const string& getSource() const;
    public: const string& getTarget() const;
    
    // Inherited from StreamDeserializer //
    public: void deserialize(PPtr<ObjectToken> headToken);
    
    // Inherited from SerializingStreamer //
    public: void serialize(PPtr<ObjectSerializer> serializer) const;
    
  };
  
} // namespace are
} // namespace knorba

#endif /* defined(__AgentRuntimeEnvironment__PeerElement__) */
