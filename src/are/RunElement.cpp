//
//  RunElement.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 9/17/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/Logger.h>
#include <kfoundation/ManagedArray.h>
#include <kfoundation/ObjectStreamReader.h>

// Internal
#include "OpcodeTable.h"
#include "MessageElement.h"
#include "PeerElement.h"

// Self
#include "RunElement.h"

namespace knorba {
namespace are {
  
// --- STATIC FIELDS --- //
  
  const string RunElement::RUN     = "Run";
  const string RunElement::CLASS   = "class";
  const string RunElement::ALIAS   = "alias";
  
  
  
// --- (DE)CONSTRUCTORS --- //
  
  RunElement::RunElement()
  {
    // Nothing
  }
  
  
// --- METHODS --- //
  
  bool RunElement::hasAlias() const {
    return _hasAlias;
  }
  
  
  const string& RunElement::getAlias() const {
    return _alias;
  }
  
  
  const string& RunElement::getClassName() const {
    return _class;
  }
  
  
  
// Inherite from StreamDeserializer //
  
  void RunElement::deserialize(PPtr<ObjectToken> headToken) {
    headToken->validateClass(RUN);
    
    Ptr<Token> token = headToken->next();
    
    while(token->is(Token::ATTRIBUTE)) {
      PPtr<AttributeToken> attrib = token.AS(AttributeToken);
      if(attrib->checkName(CLASS)) {
        _class = attrib->getValue();
      } else if(attrib->checkName(ALIAS)) {
        _alias = attrib->getValue();
        _hasAlias = true;
      } else {
        throw ParseException("Invalid attributed name \"" + attrib->getName()
            + "\"", attrib->codeRange);
      }
      token = token->next();
    }
  }
  
  
// Inherited from SerializingStreamer //
  
  void RunElement::serialize(PPtr<ObjectSerializer> serializer) const {
    serializer->object(RUN);
    serializer->attribute(CLASS, _class);
    
    if(hasAlias()) {
      serializer->attribute(ALIAS, _alias);
    }
    
    serializer->endObject();
  }
  
} // are
} // knorba