//
//  ImageLoaderAgent.cpp
//  AgentRuntimeEnvironment
//
//  Created by Kay Khandan on 5/29/15.
//  Copyright (c) 2015 Kay Khandan. All rights reserved.
//

#include <knorba/type/all.h>
#include "ImageLoaderAgent.h"

const SPtr<KRecordType> ImageLoaderAgent::PIXEL_T;
const SPtr<KGridType> ImageLoaderAgent::IMAGE_T;


SPtr<KRecordType> ImageLoaderAgent::pixel_t() {
  if(PIXEL_T.isNull()) {
    PIXEL_T = new KRecordType("knorba.image.pixel");
    PIXEL_T->addField("r", KType::OCTET)
           ->addField("g", KType::OCTET);
           ->addField("b", KType::OCTET);
           ->addField("a", KType::OCTET);
  }
  return PIXEL_T;
}


SPtr<KGridType> ImageLoaderAgent::image_t() {
  if(IMAGE_T.isNull()) {
    IMAGE_T = new KGridType(pixel_t(), 2);
  }
  return IMAGE_T;
}






const SPtr<KString> ImageLoaderAgent::OP_SETFILE = new KString("dataflow.loader.set-file");
const SPtr<KString> ImageLoaderAgent::OP_DATA_Q  = new KString("dataflow.loader.data-q");
const SPtr<KString> ImageLoaderAgent::OP_DATA_A  = new KString("dataflow.loader.data-a");

const SPtr<KString> ImageLoaderAgent::R_INPUT = new KString("input");

ImageLoaderAgent::ImageLoaderAgent(Runtime& rt, k_guid_t& guid) : Agent(rt, guid){
  registerHandler((handler_t)&ImageLoaderAgent::OpSetFile, OP_SETFILE);
  
}

void ImageLoaderAgent::opSetFile(PPtr<Message> msg){
  _filename = msg->getPayload().AS(KString);
}

void ImageLoaderAgent::opDataQ(PPtr<Message> msg){
  Ptr<KGridBasic> data = new KBasicGrid(image_t());
  
  
  respond(msg, OP_DATA_A, data.AS(KValue));
}




void ImageLoaderAgent::handlePeerConnectionRequest(PPtr<KString> role,
                                                    const k_guid_t& guid)
{
  if(role->equals(R_INPUT)) {
    addPeer(R_INPUT, guid);
  }
}



KF_EXPORT
void init(Runtime& rt) {
  rt.registerMessageFormat(ImageLoaderAgent::OP_SETFILE, KType::KString);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new ImageLoaderAgent(rt, guid);
}