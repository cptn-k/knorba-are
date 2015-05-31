//
//  tester.cpp
//  AgentRuntimeEnvironment
//
//  Created by Hamed KHANDAN on 11/27/14.
//  Copyright (c) 2014 Kay Khandan. All rights reserved.
//

#include <knorba/type/KGrid.h>
#include <Knorba/type/KType.h>

#include "Circle.h"

int main() {
  
  Ptr<KGridVector> vec = new KGridVector(Circle::type().AS(KType));
  
  int n = rand()%1000;

  
  
}