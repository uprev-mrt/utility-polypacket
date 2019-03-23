/**
  *@file poly_packet.c
  *@brief implementation for variable packet module
  *@author Jason Berger
  *@date 02/19/"2019
  */

#include "poly_packet.h"
#include <assert.h>



poly_packet_desc_t* poly_packet_desc_init(poly_packet_desc_t* desc, const char* name, int maxFields)
{
  static int id =0;

  //build out new packet descriptor
  desc->mTypeId = id++;
  desc->mName = name;
  desc->mMaxFields = maxFields;
  desc->mFieldCount =0;
  desc->mFields = (poly_field_desc_t**) malloc(sizeof(poly_field_desc_t*) * desc->mMaxFields);
  desc->mRequirementMap = (bool*) malloc(sizeof(bool) * desc->mMaxFields);
  desc->mMaxPacketSize = PACKET_METADATA_SIZE;

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

    //add to max size
    if(fieldDesc->mLen > 1)
    {
      //array fields have a leading byte with their length
      desc->mMaxPacketSize += (fieldDesc->mObjSize * fieldDesc->mLen) + 1;
    }
    else
    {
      desc->mMaxPacketSize += fieldDesc->mObjSize;
    }

    //recalculate manifest size
    desc->mManifestSize = (desc->mFieldCount +8)/8;
  }
}


void poly_packet_build(poly_packet_t* packet, poly_packet_desc_t* desc, bool allocate )
{
  packet->mDesc = desc;
  packet->mInterface = 0;
  packet->mHeader.mTypeId = desc->mTypeId;
  packet->mHeader.mToken = rand() & 0x7FFF;
  packet->mAckType = ACK_TYPE_NONE;//ACK_TYPE_TOKEN;
  packet->f_mAckCallback = NULL;
  packet->f_mFailedCallback = NULL;

  packet->mFields = (poly_field_t*) malloc(sizeof(poly_field_t) * desc->mFieldCount);
  packet->mBuilt = true;
  for(int i=0; i< desc->mFieldCount; i++)
  {
    poly_field_init(&packet->mFields[i], desc->mFields[i], allocate);
  }
}


void poly_packet_clean(poly_packet_t* packet)
{
  //if it hasnt been built yet, there is nothing to clean
  if(!packet->mBuilt)
    return;

  //destroy all fields
  for(int i=0; i < packet->mDesc->mFieldCount; i++)
  {
    poly_field_destroy(&packet->mFields[i]);
  }

  //free fields
  free(packet->mFields);

  packet->mBuilt = false;
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


ParseStatus_e poly_packet_parse_buffer(poly_packet_t* packet, uint8_t* data, int len)
{
  int idx=0;                //cursor in data
  int expectedLen =0;       //length indicated in header
  int manifestBit =0;        //bit offset for manifest

  poly_packet_desc_t* desc = packet->mDesc;

  uint8_t manifestByte;         //current manifest byte

  uint16_t checkSumComp =CHECKSUM_SEED; //calculated checksum


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

    if(idx > len)
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
  packet->mHeader.mCheckSum = CHECKSUM_SEED;
  int manifestBit =0;        //bit offset for manifest
  uint8_t* manifestByte;         //current manifest byte

  //Skip header for now, this will be written in after we get length and checksum
  idx+=sizeof(poly_packet_hdr_t);

  //create manifest
  for(int i=0; i < packet->mDesc->mFieldCount; i++)
  {
    //if fields are required, they are not in the manifest
    if(!packet->mDesc->mRequirementMap[i])
    {

      //if manifest bit is 0, we are either just starting, or rolling over
      if(manifestBit == 0)
      {
        manifestByte = &data[idx++];
        *manifestByte = 0;
      }

      if(packet->mFields[i].mPresent)
      {
        *manifestByte |= (0x80) >> manifestBit;
      }


            //increment and roll over manifest bit
            manifestBit++;
            if(manifestBit == 8)
              manifestBit =0;
    }
  }

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
  for(int i = sizeof(poly_packet_hdr_t) ; i < idx; i++)
  {
    packet->mHeader.mCheckSum +=  data[i];
  }

  memcpy((void*)&data[0], (void*)&packet->mHeader, sizeof(poly_packet_hdr_t));

  return idx;
}

int poly_packet_print_json(poly_packet_t* packet, char* buf, bool printHeader)
{
  int idx =0;
  poly_field_t* field;
  bool first = true;

  //this generally means we built the packet and its never been packed,
  // so the header hasnt been set
  if(packet->mHeader.mCheckSum == 0)
  {
    poly_packet_update_header(packet);
  }

  idx+= MRT_SPRINTF(&buf[idx],"{");

  idx += MRT_SPRINTF(&buf[idx]," \"packetType\" : \"%s\" ,", packet->mDesc->mName);

  if(printHeader)
  {
    idx += MRT_SPRINTF(&buf[idx]," \"typeId\" : \"%02X\" ,", packet->mHeader.mTypeId);
    idx += MRT_SPRINTF(&buf[idx]," \"token\" : \%04X\" ,", packet->mHeader.mToken);
    idx += MRT_SPRINTF(&buf[idx]," \"checksum\" : \"%04X\" ,", packet->mHeader.mCheckSum);
    idx += MRT_SPRINTF(&buf[idx]," \"len\" : %d ", packet->mHeader.mDataLen);
  }

  if(packet->mDesc->mFieldCount > 0)
  {
    idx += MRT_SPRINTF(&buf[idx]," ,");
  }

  for(int i=0; i < packet->mDesc->mFieldCount; i++)
  {
    field = &packet->mFields[i];

    if(field->mPresent)
    {
      if(!first)
        idx+= MRT_SPRINTF(&buf[idx]," , ");

      idx+= poly_field_print_json(field, &buf[idx]);

      first = false;
    }

  }

  idx+= MRT_SPRINTF(&buf[idx],"}");

  return idx;
}

int poly_packet_update_header(poly_packet_t* packet)
{

  uint8_t data[packet->mDesc->mMaxPacketSize];

  //easiest way to get the meta data is to pack the message
  //Its not very effecient but this should be a rare use-case, mainly for debugging
  int len = poly_packet_pack(packet, data);

  return len;
}
