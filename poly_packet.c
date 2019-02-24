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
  desc->mTypeId = id++;
  desc->mName = name;
  desc->mMaxFields = maxFields;
  desc->mFieldCount =0;
  desc->mFields = (poly_field_desc_t**) malloc(sizeof(poly_field_desc_t*) * desc->mMaxFields);
  desc->mRequirementMap = (bool*) malloc(sizeof(bool) * desc->mMaxFields);

  return desc;
}

void poly_packet_desc_add_field(poly_packet_desc_t* desc, poly_field_desc_t* fieldDesc, bool required)
{
  if(desc->mFieldCount < desc->mMaxFields)
  {
    //add field desc to poly_packet_desc
    desc->mRequirementMap[desc->mFieldCount] = required;
    desc->mFields[desc->mFieldCount] = fieldDesc;

    //increment field count
    desc->mFieldCount++;

    //recalculate manifest size
    desc->mManifestSize = (desc->mFieldCount +8)/8;
  }
}

poly_packet_t* new_poly_packet(poly_packet_desc_t* desc)
{
  poly_packet_t* newPacket = (poly_packet_t*)malloc(sizeof(poly_packet_t));
  newPacket->mBound = false;
  newPacket->mAllocated = false;
  newPacket->mDesc = desc;

  newPacket->mFields = (poly_field_t*) malloc(sizeof(poly_field_t) * desc->mFieldCount);
  for(int i=0; i< desc->mFieldCount; i++)
  {
    poly_field_init(&newPacket->mFields[i], desc->mFields[i]);
  }

  return newPacket;
}

void poly_packet_destroy(poly_packet_t* packet)
{
  if(packet->mAllocated)
  {
    for(int i=0; i < packet->mDesc->mFieldCount; i++)
    {
      free(packet->mFields[i].mData);
    }

    free(packet->mFields);
  }

  free(packet);
}

poly_field_t* poly_packet_get_field(poly_packet_t* packet, poly_field_desc_t* desc)
{
  for(int i=0; i < packet->mDesc->mFieldCount; i++ )
  {
    if(packet->mDesc->mFields[i] == desc)
    {
      return &packet->mFields[i];
    }
  }

  return NULL;
}



int poly_packet_id(uint8_t* data, int len)
{
  poly_packet_hdr_t* hdr;
  if(len < sizeof(poly_packet_hdr_t))
  {
    return -1;
  }

  hdr = (poly_packet_hdr_t*) data;
  return (int)hdr->mTypeId;
}


ePacketStatus poly_packet_parse(poly_packet_t* packet, poly_packet_desc_t* desc, uint8_t* data, int len)
{
  int idx=0;                //cursor in data
  int expectedLen =0;       //length indicated in header

  int manifestBit =0;        //bit offset for manifest
  int manifestByte;         //current manifest byte

  uint16_t checkSumComp =0; //calculated checksum

  packet->mDesc = desc;     //assign descriptor to packet

  //packet must be at least as long as the meta data required for each packet
  if(len < PACKET_METADATA_SIZE)
  {
    return PACKET_INCOMPLETE;
  }

  //copy over header information
  memcpy((void*)&packet->mHeader, (void*)data, sizeof(poly_packet_hdr_t));
  idx += sizeof(poly_packet_hdr_t);

  //get expected size of packet from header
  expectedLen = packet->mHeader.mDataLen + PACKET_METADATA_SIZE;

  //if sizes do not match, there is an error
  if(len < expectedLen)
  {
    return PACKET_INCOMPLETE;
  }
  else if(len > expectedLen)
  {
      return PACKET_PARSING_ERROR;
  }

  //mark all fields as preset/absent
  for(int i=0; i < desc->mFieldCount; i++)
  {
    //if fields are required, they are not in the manifest
    if(desc->mRequirementMap[i])
    {
      packet->mFields[i].mPresent = true;
    }
    else
    {
      //if manifest bit is 0, we are either just starting, or rolling over
      //grab next byte as part of manifest
      if(manifestBit == 0)
      {
        manifestByte = data[idx++];
      }

      //check manifest for optional fields
      if((manifestByte << manifestBit) & 0x80)
      {
        packet->mFields[i].mPresent = true;
      }
      else
      {
        packet->mFields[i].mPresent = false;
      }

      //increment and roll over manifest bit
      manifestBit++;
      if(manifestBit == 8)
        manifestBit =0;

    }
  }

  //Parse data from all fields
  for(int i=0; i < desc->mFieldCount; i++)
  {
    idx+= poly_field_parse(&packet->mFields[i], &data[idx]);

    if(idx >= len)
    {
      return PACKET_PARSING_ERROR;
    }
  }

  // validate checksum
  for(int i=0; i < packet->mHeader.mDataLen; i++)
  {
    checkSumComp += data[sizeof(poly_packet_hdr_t) +i];
  }

  if(packet->mHeader.mCheckSum != checkSumComp)
  {
    return PACKET_BAD_CHECKSUM;
  }

  return PACKET_VALID;
}

int poly_packet_pack(poly_packet_t* packet, uint8_t* data)
{
  int idx=0;
  poly_field_t* field;
  packet->mHeader.mCheckSum =0;

  //Skip header for now, this will be written in after we get length and checksum
  idx+=sizeof(poly_packet_hdr_t);

  //copy fields
  for(int i=0; i < packet->mDesc->mFieldCount; i++)
  {
    field = &packet->mFields[i];
    if(field->mPresent)
    {
      //if its a variable length field, put int length first (max 255)
      if(field->mDesc->mVarLen)
      {
        data[idx++] = field->mSize;
      }

      memcpy(&data[idx], field->mData, field->mSize);
      idx+= field->mSize;
    }

  }

  //fill out header
  packet->mHeader.mDataLen = idx - sizeof(poly_packet_hdr_t);

  //get checksum
  for(int i =0; i < idx; i++)
  {
    packet->mHeader.mCheckSum +=  data[i + sizeof(poly_packet_hdr_t)];
  }

  memcpy((void*)&data[0], (void*)&packet->mHeader, sizeof(poly_packet_hdr_t));
}

int poly_packet_print_json(poly_packet_t* packet, char* buf, bool printMeta)
{
  int idx =0;
  poly_field_t* field;

  idx+= sprintf(&buf[idx],"{");

  if(printMeta)
  {
    idx += sprintf(&buf[idx]," \"typeId\" : \"%02X\" ,", packet->mHeader.mTypeId);
    idx += sprintf(&buf[idx]," \"token\" : \"%04X\" ,", packet->mHeader.mToken);
    idx += sprintf(&buf[idx]," \"checksum\" : \"%04X\" ,", packet->mHeader.mCheckSum);
    idx += sprintf(&buf[idx]," \"len\" : \"%d\" , ", packet->mHeader.mDataLen);
  }

  for(int i=0; i < packet->mDesc->mFieldCount; i++)
  {
    if(i > 0)
      idx+= sprintf(&buf[idx]," , ");

    field = &packet->mFields[i];

    if(field->mPresent)
    {
      idx+= poly_field_print_json(field, &buf[idx]);
    }
    printf(" SOFAR: %s\n",buf);
  }

  idx+= sprintf(&buf[idx],"}");

  return idx;
}