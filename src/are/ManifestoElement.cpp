//
//  Manifesto.cpp
//  AgentRuntimeEnvironment
//
//  Created by Kay Khandan on 11/8/14.
//  Copyright (c) 2014 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/ObjectStreamReader.h>
#include <kfoundation/XmlObjectStreamReader.h>
#include <kfoundation/FileInputStream.h>
#include <kfoundation/ManagedArray.h>

// Internal
#include "OpcodeTable.h"
#include "LoadElement.h"
#include "RunElement.h"
#include "RuntimeBase.h"
#include "DynamicAgentLoader.h"
#include "MessageElement.h"
#include "PeerElement.h"

// Self
#include "ManifestoElement.h"

namespace knorba {
namespace are {

// --- STATIC FIELDS --- //
  
  const string ManifestoElement::MANIFESTO = "Manifesto";
  
  
// --- (DE)CONSTRUCTORS --- //
  
  ManifestoElement::ManifestoElement(RuntimeBase& rt)
  : _rt(rt)
  {
    _loadElements = new ManagedArray<LoadElement>();
    _runElements = new ManagedArray<RunElement>();
    _peerElements = new ManagedArray<PeerElement>();
    _messageElements = new ManagedArray<MessageElement>();
  }
  
  
// --- METHODS --- //
  
  PPtr< ManagedArray<LoadElement> > ManifestoElement::getLoadElements() const {
    return _loadElements;
  }
  
  
  PPtr< ManagedArray<RunElement> > ManifestoElement::getRunElements() const {
    return _runElements;
  }
  
  
  PPtr< ManagedArray<PeerElement> > ManifestoElement::getPeerElements() const {
    return _peerElements;
  }
  
  
  PPtr< ManagedArray<MessageElement> > ManifestoElement::getMessageElements() const {
    return _messageElements;
  }

  
// Inherited from StreamDeserializer //
  
  void ManifestoElement::deserialize(PPtr<ObjectToken> headToken) {
    headToken->validateClass(MANIFESTO);
    
    Ptr<Token> token = headToken->next();
    
    while(token->is(Token::OBJECT)) {
      PPtr<ObjectToken> obj = token.AS(ObjectToken);
      
      if(obj->checkClass(LoadElement::LOAD)) {
        Ptr<LoadElement> e = new LoadElement();
        e->deserialize(obj);
        _loadElements->push(e);
        _rt.loadClass(e->getClassName());
      } else if(obj->checkClass(RunElement::RUN)) {
        Ptr<RunElement> e = new RunElement();
        e->deserialize(obj);
        _runElements->push(e);
      } else if(obj->checkClass(MessageElement::MESSAGE)) {
        Ptr<MessageElement> e = new MessageElement(_rt);
        e->deserialize(obj);
        _messageElements->push(e);
      } else if(obj->checkClass(PeerElement::CONNECT)) {
        Ptr<PeerElement> e = new PeerElement();
        e->deserialize(obj);
        _peerElements->push(e);
      } else {
        obj->throwInvlaidClass();
      }
      
      token = token->next();
    }
    
    token->validateType(Token::END_OBJECT);
  }
  
  
// Inherited from SerializingStreamer //
  
  void ManifestoElement::serialize(PPtr<ObjectSerializer> serializer) const {
    serializer->object(MANIFESTO);
    for(int i = 0; i < _loadElements->getSize(); i++) {
      serializer->member("")->object<LoadElement>(_loadElements->at(i));
    }
    for(int i = 0; i < _runElements->getSize(); i++) {
      serializer->member("")->object<RunElement>(_runElements->at(i));
    }
    for(int i = 0; i < _messageElements->getSize(); i++) {
      serializer->member("")->object<MessageElement>(_messageElements->at(i));
    }
    for(int i = 0; i < _peerElements->getSize(); i++) {
      serializer->member("")->object<PeerElement>(_peerElements->at(i));
    }
    serializer->endObject();
  }
  
  
} // namespace are
} // namespace knorba
