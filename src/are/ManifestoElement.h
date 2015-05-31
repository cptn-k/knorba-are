//
//  Manifesto.h
//  AgentRuntimeEnvironment
//
//  Created by Kay Khandan on 11/8/14.
//  Copyright (c) 2014 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__Manifesto__
#define __AgentRuntimeEnvironment__Manifesto__

// Super
#include <kfoundation/ManagedObject.h>
#include <kfoundation/StreamDeserializer.h>
#include <kfoundation/SerializingStreamer.h>

namespace kfoundation {
  class FileInputStream;
  class XmlObjectStreamReader;
}

namespace knorba {
namespace are {

  using namespace kfoundation;
  
  class Bundle;
  class RuntimeBase;
  class LoadElement;
  class RunElement;
  class PeerElement;
  class MessageElement;
  
  class ManifestoElement : public ManagedObject, public  StreamDeserializer,
      public SerializingStreamer
  {
    
  // --- STATIC FIELDS --- //
    
    private: static const string MANIFESTO;
    
    
  // --- FIELDS --- //
    
    private: RuntimeBase& _rt;
    private: Ptr< ManagedArray<LoadElement> > _loadElements;
    private: Ptr< ManagedArray<RunElement> >  _runElements;
    private: Ptr< ManagedArray<PeerElement> > _peerElements;
    private: Ptr< ManagedArray<MessageElement> > _messageElements;
    
    
  // --- (DE)CONSTURCTORS --- //
    
    public: ManifestoElement(RuntimeBase& rt);
    
    
  // --- METHODS --- //
    
    public: PPtr< ManagedArray<LoadElement> > getLoadElements() const;
    public: PPtr< ManagedArray<RunElement> > getRunElements() const;
    public: PPtr< ManagedArray<PeerElement> > getPeerElements() const;
    public: PPtr< ManagedArray<MessageElement> > getMessageElements() const;
    
    // Inherited from StreamDeserializer //
    public: void deserialize(PPtr<ObjectToken> token);
    
    // Inherited from SerializingStreamer //
    public: void serialize(PPtr<ObjectSerializer> serializer) const;
    
  };
  
} // namespace are
} // namespace knorba


#endif /* defined(__AgentRuntimeEnvironment__Manifesto__) */