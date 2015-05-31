//
//  Message.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 9/17/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/Logger.h>
#include <kfoundation/ObjectStreamReader.h>

// KnoRBA
#include <knorba/Runtime.h>
#include <knorba/type/KType.h>
#include <knorba/type/KValue.h>
#include <knorba/type/KString.h>

// Internal
#include "OpcodeTable.h"
#include "AreException.h"

// Self
#include "MessageElement.h"


namespace knorba {
namespace are {

// --- STATIC FIELDS --- //
  
  const string MessageElement::MESSAGE = "Message";
  const string MessageElement::OPCODE  = "opcode";
  const string MessageElement::TARGET = "target";
  
  
// --- (DE)CONSTRUCTORS --- //
  
  MessageElement::MessageElement(Runtime& rt)
  : _rt(rt)
  {
    // Nothing;
  }
  
  
  
// --- METHODS --- //

  const string& MessageElement::getTarget() const {
    return _target;
  }
  
  
  PPtr<KString> MessageElement::getOpcode() const {
    return _opcode;
  }
  
  
  PPtr<KValue> MessageElement::getPayload() const {
    return _payload;
  }
  
  
// Inherited from StreamDeserializer //
  
  void MessageElement::deserialize(PPtr<ObjectToken> headToken) {
    headToken->validateClass(MESSAGE);
    
    Ptr<Token> token = headToken->next();
    
    while(token->is(Token::ATTRIBUTE)) {
      Ptr<AttributeToken> attrib = token.AS(AttributeToken);
      if(attrib->checkName(OPCODE)) {
        _opcode = KS(attrib->getValue());
      } else if(attrib->checkName(TARGET)) {
        _target = attrib->getValue();
      } else {
        attrib->throwInvliadName();
      }
      
      token = token->next();
    }
    
    if(_opcode.isNull()) {
      headToken->throwMissingAttribute(OPCODE);
    }
    
    if(_target.empty()) {
      headToken->throwMissingAttribute(TARGET);
    }
    
    if(token->is(Token::OBJECT)) {
      Ptr<KType> type = _rt.getMessageFormatByHash(_opcode->getHashCode());
      if(type.isNull()) {
        throw AreException("Could not resolve message type for opcode: \""
            + _opcode->toUtf8String() + "\"");
      }
      _payload = type->instantiate();
      _payload->readFromObjectStream(token.AS(ObjectToken));      
      token = token->next();
    }
    
    token->validateType(Token::END_OBJECT);
  }
  
  
// Inherited from SerializingStreamer //
  
  void MessageElement::serialize(PPtr<ObjectSerializer> serializer) const {
    serializer->object(MESSAGE)
        ->attribute(OPCODE, _opcode->toUtf8String())
        ->member("payload")->object<KValue>(_payload)
        ->endObject();
  }

} // are
} // knorba

