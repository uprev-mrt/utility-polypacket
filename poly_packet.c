/**
  *@file poly_packet.c
  *@brief implementation for variable packet module
  *@author Jason Berger
  *@date 02/19/"2019
  */

#include "poly_packet.h"




poly_packet_desc_t* new_poly_packet_desc(const char* name, int maxFields)
{
  static int id =0;

  //build out new packet descriptor
  poly_packet_desc_t* desc = (poly_packet_desc_t*)malloc(sizeof(poly_packet_desc_t));
  desc->mPayloadId = id++;
  desc->mName = name;
  desc->mMaxFields = maxFields;
  desc->mFieldCount =0;
  mFields = (poly_field_desc_t*) malloc(sizeof(poly_field_desc_t*) * desc->mMaxFields);

  return desc;
}

void poly_packet_desc_add_field(poly_packet_desc_t* desc, poly_field_desc_t* fieldDesc)
{
  if(desc->mFieldCount < desc->mMaxFields)
  {
    //add field desc to poly_packet_desc
    desc->mFields[desc->mFieldCount++] = fieldDesc;

    desc->mManifestSize = (desc->mFieldCount +8)/8;
  }
}


int poly_packet_id(uin8_t* data, int len)
{
  poly_packet_hdr_t* hdr;
  if(len < sizeof(poly_packet_hdr_t))
  {
    return -1
  }

  hdr = (poly_packet_hdr_t*) data;
  return (int)hdr->mTypeId;
}


ePacketStatus poly_packet_parse(poly_packet_t* packet, poly_packet_desc_t* dec, uin8_t* data, int len)
{
  int idx=0;                //cursor in data
  int expectedLen =0;       //length indicated in header

  int bitMask =1;           //bit mask for manifest
  int manifestByte;         //current manifest byte

  uint16_t checkSumComp =0; //calculated checksum
}
