//
//  AgentMetadata.h
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 11/7/14.
//  Copyright (c) 2014 Kay Khandan. All rights reserved.
//

#ifndef __AgentRuntimeEnvironment__AgentMetadata__
#define __AgentRuntimeEnvironment__AgentMetadata__

// KFoundation
#include <kfoundation/ManagedObject.h>
#include <kfoundation/SerializingStreamer.h>
#include <kfoundation/Array.h>
#include <kfoundation/ObjectStreamReader.h>
#include <kfoundation/Path.h>


// KnoRBA
#include <knorba/AgentLoader.h>

namespace knorba {
  namespace are {
    
    using namespace kfoundation;
    
    class AgentMetadata : public ManagedObject, public SerializingStreamer {
      
    // --- NESTED DATATYPES --- //
      
      typedef void (*init_t)(Runtime& rt);
      typedef Agent* (*instantiate_t)(Runtime& rt, const k_guid_t& guid);
      
      private: class Loader : public AgentLoader {
      
      // --- FIELDS --- //
      
        private: void* _handle;
        private: init_t _initFn;
        private: instantiate_t _instantiateFn;
        private: kf_octet_t _managerIndex;
      
      
      // --- (DE)CONSTRUCTOR --- //
      
        public: Loader(PPtr<Path> binary, const string& name,
                       const string& bundleName, PPtr<Path> resources);
          
        public: ~Loader();
      
      
      // --- METHODS --- //
      
        public: void init(Runtime& rt);
        public: Agent* instantiate(Runtime& rt, const k_guid_t& guid);
      
      };
      
      
    // --- STATIC FIELDS --- //
      
      private: static const string AGENT;
      private: static const string NAME;
      private: static const string SOURCE;
      private: static const string PATH;
      private: static const string EXTENSION;
      
      
    // --- FIELDS --- //
      
      private: string _name;
      private: Ptr<Path> _binary;
      private: Ptr<Path> _resources;
      private: Array<string>::Ptr_t _sources;
      private: Ptr<AgentLoader> _loader;
      
      
    // --- (DE)CONSTRUCTORS --- //
      
      public: AgentMetadata();
      public: ~AgentMetadata();
      
      
    // --- METHODS --- //
      
      public: void setName(const string& name);
      public: const string& getName() const;
      public: void addSource(const string& path);
      public: void clearSources();
      public: Array<string>::PPtr_t getSources() const;
      public: bool hasSource() const;
      public: void setBinaryPath(PPtr<Path> path);
      public: PPtr<Path> getBinaryPath() const;
      public: PPtr<AgentLoader> getLoader() const;
      public: void compile(string command, PPtr<Path> target);
      public: void readFromObjectStream(PPtr<ObjectToken> headToken,
          const string& bundleName, PPtr<Path> bundleRoot);
      
      // Inherited from SeializingStreamer
      public: void serialize(PPtr<ObjectSerializer> serializer) const;
      
    };
    
  }
}

#endif /* defined(__AgentRuntimeEnvironment__AgentMetadata__) */
