//
//  PeerElement.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 3/31/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/ObjectSerializer.h>
#include <kfoundation/ObjectStreamReader.h>

// Self
#include "PeerElement.h"

namespace knorba {
namespace are {
  
// --- STATIC FIELDS --- //
  
  const string PeerElement::CONNECT = "Connect";
  const string PeerElement::SOURCE = "source";
  const string PeerElement::TARGET = "target";
  const string PeerElement::ROLE = "role";
  
  
// --- (DE)CONSTRUCTORS --- //
  
  PeerElement::PeerElement() {
    // Nothing;
  }
  
  
// --- METHODS --- //
  
  const string& PeerElement::getRole() const {
    return _role;
  }
  
  
  const string& PeerElement::getSource() const {
    return _source;
  }
  
  
  const string& PeerElement::getTarget() const {
    return _target;
  }
  
  
// Inherited from StreamDeserializer //
  
  void PeerElement::deserialize(PPtr<ObjectToken> headToken) {
    headToken->validateClass(CONNECT);
    
    Ptr<Token> token = headToken->next();
    
    while(token->is(Token::ATTRIBUTE)) {
      PPtr<AttributeToken> attrib = token.AS(AttributeToken);
      if(attrib->checkName(SOURCE)) {
        _source = attrib->getValue();
      } else if(attrib->checkName(TARGET)) {
        _target = attrib->getValue();
      } else if(attrib->checkName(ROLE)) {
        _role = attrib->getValue();
      } else {
        attrib->throwInvliadName();
      }
      
      token = token->next();
    }
    
    if(_role.empty()) {
      headToken->throwMissingAttribute(ROLE);
    }
    
    if(_source.empty()) {
      headToken->throwMissingAttribute(SOURCE);
    }
    
    if(_target.empty()) {
      headToken->throwMissingAttribute(TARGET);
    }
    
    token->validateType(Token::END_OBJECT);
  }
  
  
// Inherited from SerializingStreamer //
  
  void PeerElement::serialize(PPtr<ObjectSerializer> serializer) const {
    serializer->object(CONNECT)
      ->attribute(ROLE, _role)
      ->attribute(SOURCE, _source)
      ->attribute(TARGET, _target)
      ->endObject();
  }
  
} // namespace are
} // namespace knorba