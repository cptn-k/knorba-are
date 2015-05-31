//
//  LoadElement.cpp
//  AgentRuntimeEnvironment
//
//  Created by Kay Khandan on 3/22/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

// Std
#include <string>

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/ObjectStreamReader.h>

// Self
#include "LoadElement.h"

namespace knorba {
namespace are {
  
// --- STATIC FIELDS --- //
  
  const string LoadElement::LOAD = "Load";
  const string LoadElement::SOURCE = "Source";
  const string LoadElement::PATH = "path";
  const string LoadElement::CLASS = "class";
  
  
  
// --- CONSTRUCTOR --- //
  
  LoadElement::LoadElement() {
    // Nothing;
  }
  
  
// --- METHODS --- //
  
  const string& LoadElement::getClassName() const {
    return _className;
  }
  
  
  PPtr< Array<string> > LoadElement::getSourceFiles() const {
    return _sourceFiles;
  }
  

// Inherited from StreamDeserializer //
  
  void LoadElement::deserialize(PPtr<ObjectToken> headToken) {
    headToken->validateClass(LOAD);
    Ptr<Token> token = headToken->next();
    if(token->is(Token::ATTRIBUTE)) {
      PPtr<AttributeToken> attrib = token.AS(AttributeToken);
      attrib->validateName(CLASS);
      _className = attrib->getValue();
    }
    
    token = token->next();
    
    while(token->is(Token::OBJECT)) {
      PPtr<ObjectToken> object = token.AS(ObjectToken);
      object->validateClass(SOURCE);
      
      if(_sourceFiles.isNull()) {
        _sourceFiles = new Array<string>();
      }
      
      const string& path = token->next()->asAttribute()->validateName(PATH)->getValue();
      _sourceFiles->push(path);
      
      token = token->next();
      token->validateType(Token::END_OBJECT);
      
      token = token->next();
    }
    
    token->validateType(Token::END_OBJECT);
  }
  
  
// Inherited from SerializingStreamer //
  
  void LoadElement::serialize(PPtr<ObjectSerializer> serializer) const {
    serializer->object(LOAD)
        ->attribute(CLASS, _className);
    
    if(!_sourceFiles.isNull()) {
      for(int i = 0; i < _sourceFiles->getSize(); i++) {
        serializer->object(SOURCE)
            ->attribute(PATH, _sourceFiles->at(i))
            ->endObject();
      }
    }
    
    serializer->endObject();
  }
  
} // namespace are
} // namespace knorba


#include "LoadElement.h"
