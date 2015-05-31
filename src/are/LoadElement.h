//
//  LoadElement.h
//  AgentRuntimeEnvironment
//
//  Created by Kay Khandan on 3/22/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__LoadElement__
#define __AgentRuntimeEnvironment__LoadElement__

#include <kfoundation/ManagedObject.h>
#include <kfoundation/SerializingStreamer.h>
#include <kfoundation/StreamDeserializer.h>
#include <kfoundation/Array.h>

namespace knorba {
namespace are {
  
  using namespace std;
  using namespace kfoundation;
  
  class LoadElement : public ManagedObject, public SerializingStreamer,
      public StreamDeserializer
  {
    
  // --- STATIC FIELDS --- //
    
    public: static const string LOAD;
    public: static const string SOURCE;
    public: static const string PATH;
    public: static const string CLASS;
    
    
  // --- FIELDS --- //
    
    private: string _className;
    private: Ptr< Array<string> > _sourceFiles;
    
    
  // --- CONSTRUCTORS --- //
    
    public: LoadElement();
    
    
  // --- METHODS --- //
    
    public: const string& getClassName() const;
    public: PPtr< Array<string> > getSourceFiles() const;
    
    // Inherited from StreamDeserializer
    public: void deserialize(PPtr<ObjectToken> headToken);
    
    // Inherited from SerializingStreamer //
    public: void serialize(PPtr<ObjectSerializer> serializer) const;
    
  };
  
} // namespace are
} // namespace knorba

#endif /* defined(__AgentRuntimeEnvironment__LoadElement__) */
