//
//  AgentRuntimeEnvironment.cpp
//  KnoRBA-ARE-XCodeWrapper
//
//  Created by Hamed KHANDAN on 8/14/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

// Std
#include <string>

// KnoRBA
#include <knorba/type/KRecord.h>

// KFoundation
#include <kfoundation/Ptr.h>
#include <kfoundation/Logger.h>
#include <kfoundation/Path.h>
#include <kfoundation/MasterMemoryManager.h>

// Internal
#include "Arguments.h"
#include "KernelRuntime.h"

using namespace std;
using namespace kfoundation;
using namespace knorba::are;
using namespace knorba::type;

enum kernel_action_t {
  START,
  STOP,
  RESTART,
  STATUS
};

//\/ actions /\////////////////////////////////////////////////////////////////

void runApp(const string& bundleName) {
  cout << "runApp(" << bundleName << ")" << endl;
}


void performKernelAction(PPtr<Path> knorbaHome, Arguments& args) {
  Ptr<KernelRuntime> rt = new KernelRuntime(knorbaHome);
  
  Ptr<KRecord> pid = rt->getPidFileContents();
  
  bool isStartCommand = args.getVerb() == Arguments::START;
  
  if(isStartCommand) {
    if(!pid.isNull()) {
      LOG_ERR << "Kernel is already running: " << *pid << EL;
      rt->shutdown();
      return;
    }
    rt->makePidFile();
  } else {
    if(pid.isNull()) {
      LOG_ERR << "Kernel is not running." << EL;
      rt->shutdown();
      return;
    }
  }
  
  try {
    if(isStartCommand) {
      Ptr<Path> bundlePath = knorbaHome->addSegement("repo/Kernel.kap");
      rt->runBundle(bundlePath);
    } else {
      Ptr<Path> bundlePath = knorbaHome->addSegement("repo/Shell.kap");
      rt->runBundle(bundlePath);
    }
    
    switch (args.getVerb()) {
      case Arguments::START:
        rt->kernelRun();
        break;
        
      case Arguments::STOP:
        rt->kernelShutdown(pid->getInteger(0));
        break;
        
      case Arguments::RESTART:
        rt->kernelRestart(pid->getInteger(0));
        break;
        
      case Arguments::RUN:
        rt->kernelRunBundle(pid->getInteger(0), args.getBundleName());
        break;
        
      case Arguments::STATUS:
        rt->kernelPrintStatus(pid->getInteger(0));
        break;
        
      default:
        LOG_ERR << "Error handling arguments: " << args << EL;
    }
    
    rt->waitWhileRunning();
    rt->shutdown();
  } catch(KFException e) {
    LOG << e << EL;
    
    LOG_ERR << "An exception occured in the main thread. Commencing safe "
        "shutdown." << EL;
    
    rt->shutdown();
  }
}


void setupLogger(PPtr<Path> knorbaHome, string name) {
  Ptr<Path> tmpDir = knorbaHome->addSegement("data");
  if(!tmpDir->exists()) {
    tmpDir->makeDir();
  }
  
  Ptr<Path> logFilePath = tmpDir->addSegement(name + ".log");
  System::getLogger().addChannel("file", logFilePath->getString());
  
  System::getLogger().getChannelByName("default")
      .setLevel(Logger::ERR)
      .setFormat(false, false, false);
}


//\/ main /\///////////////////////////////////////////////////////////////////

void k_main(Arguments& args) {
  Ptr<Path> knorbaHome = System::getExePath()->parent();

  if(args.getVerb() == Arguments::START) {
    setupLogger(knorbaHome, "kernel");
  } else {
    setupLogger(knorbaHome, "shell");
  }
  
  LOG << EL;
  LOG << "*** Begin (" << args << ") ***" << EL;
  
  if(args.getVerb() == Arguments::ERROR) {
    LOG_ERR << args.getError() << EL;
    Arguments::printUsuage(cout);
  } else {
    performKernelAction(knorbaHome, args);
  }
  
  System::getMasterMemoryManager().printStats();
  
  LOG << "*** End ***" << EL;

  cout << "ARE process terminated." << endl;
}


int main(int argc, char** argv) {
  Arguments args(argc, argv);
  k_main(args);
  return 0;
}


//\/ END /\////////////////////////////////////////////////////////////////////

