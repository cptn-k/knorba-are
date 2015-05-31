//
//  AreException.cpp
//  ARE
//
//  Created by Hamed KHANDAN on 10/8/14.
//  Copyright (c) 2014 RIKEN AICS Advanced Visualization Research Team. All rights reserved.
//

#include "AreException.h"

knorba::are::AreException::AreException(const string& message)
: kfoundation::KFException(message)
{
  // Nothing
}