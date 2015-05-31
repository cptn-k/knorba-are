//
//  AreException.h
//  ARE
//
//  Created by Hamed KHANDAN on 10/8/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#ifndef __ARE__AreException__
#define __ARE__AreException__

#include <kfoundation/KFException.h>

namespace knorba {
  namespace are {
    
    using namespace kfoundation;
    
    class AreException : public KFException {
    public:
      AreException(const string& message);
    };
    
  }
}


#endif /* defined(__ARE__AreException__) */
