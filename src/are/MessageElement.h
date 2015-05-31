//
//  Message.h
//  ARE
//
//  Created by Hamed KHANDAN on 9/17/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef __ARE__Message__
#define __ARE__Message__

// KFoundation
#include <kfoundation/ManagedObject.h>
#include <kfoundation/SerializingStreamer.h>
#include <kfoundation/StreamDeserializer.h>

namespace knorba {
  
  class Runtime;
  
namespace type {
  class KString;
  class KValue;
}
  
namespace are {
  
  using namespace kfoundation;
  using namespace knorba::type;
  
  class MessageElement : public ManagedObject, public SerializingStreamer,
      public StreamDeserializer
  {
    
  // --- STATIC FIELDS --- //
    
    public: static const string MESSAGE;
    public: static const string OPCODE;
    public: static const string TARGET;

    
  // --- FIELDS --- //
    
    private: Runtime& _rt;
    private: string _target;
    private: Ptr<KString> _opcode;
    private: Ptr<KValue>  _payload;
    
    
  // --- (DE)CONSTRUCTOR --- //
    
    public: MessageElement(Runtime& rt);
    
    
  // --- METHODS --- //
    
    public: const string& getTarget() const;
    public: PPtr<KString> getOpcode() const;
    public: PPtr<KValue> getPayload() const;
    
    // Inherited from StreamDeserializer //
    public: void deserialize(PPtr<ObjectToken> headToken);
    
    // Inherited from SerializingStreamer //
    public: void serialize(PPtr<ObjectSerializer> serializer) const;
    
  };
  
} // are
} // knorba

#endif /* defined(__ARE__Message__) */
