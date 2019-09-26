/**
  *@file poly_packet.c
  *@brief implementation for variable packet module
  *@author Jason Berger
  *@date 02/19/"2019
  */

#include "poly_packet.h"
#include <assert.h>

#if defined(POLY_PACKET_DEBUG_LVL)
char POLY_DEBUG_PRINTBUF[512];
#endif

poly_packet_desc_t* poly_packet_desc_init(poly_packet_desc_t* desc, int id,  const char* name, int maxFields)
{
  //build out new packet descriptor
  desc->mTypeId = id;
  desc->mName = name;
  desc->mMaxFields = maxFields;
  desc->mFieldCount =0;
  desc->mFields = (poly_field_desc_t**) malloc(sizeof(poly_field_desc_t*) * desc->mMaxFields);
  desc->mMaxPacketSize = PACKET_METADATA_SIZE;


  return desc;
}

poly_packet_desc_t* poly_packet_desc_deinit(poly_packet_desc_t* desc)
{
  if(desc->mMaxFields > 0)
  {
    free(desc->mFields);
  }

  return desc;
}

void poly_packet_desc_add_field(poly_packet_desc_t* desc, poly_field_desc_t* fieldDesc, bool required)
{
  if(desc->mFieldCount < desc->mMaxFields)
  {
    //add field desc to poly_packet_desc
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
  }
}


void poly_packet_build(poly_packet_t* packet, const poly_packet_desc_t* desc, bool allocate )
{

  packet->mDesc = desc;
  packet->mInterface = 0;
  packet->mHeader.mTypeId = desc->mTypeId;
  packet->mHeader.mToken = rand() & 0x7FFF;
  packet->mHeader.mCheckSum = 0;
  packet->mAckType = ACK_TYPE_NONE;// until we put in the auto/ack retry timing
  packet->f_mAckCallback = NULL;
  packet->f_mFailedCallback = NULL;
  packet->mBuilt = false;
  packet->mSpooled = false;
  MRT_MUTEX_CREATE(packet->mMutex);

  packet->mFields = (poly_field_t*) malloc(sizeof(poly_field_t) * desc->mFieldCount);
  packet->mBuilt = true;
  for(int i=0; i< desc->mFieldCount; i++)
  {
    poly_field_init(&packet->mFields[i], desc->mFields[i], allocate);
  }
}


void poly_packet_clean(poly_packet_t* packet)
{
  //destroy all fields
  for(int i=0; i < packet->mDesc->mFieldCount; i++)
  {
    poly_field_destroy(&packet->mFields[i]);
  }

  //free fields
  free(packet->mFields);

  //delete mutex
  MRT_MUTEX_DELETE(packet->mMutex);

  packet->mBuilt = false;
  packet->mSpooled = false;
}

bool poly_packet_has(poly_packet_t* packet, const poly_field_desc_t* desc)
{
  for(int i=0; i < packet->mDesc->mFieldCount; i++)
  {
    if(packet->mFields[i].mDesc == desc )
    {
      return packet->mFields[i].mPresent;
    }
  }

  return false;
}

int poly_packet_get_field(poly_packet_t* packet, const poly_field_desc_t* desc, void* data)
{
  MRT_MUTEX_LOCK(packet->mMutex);

  int ret =0;
  for(int i=0; i < packet->mDesc->mFieldCount; i++ )
  {
    if(packet->mDesc->mFields[i] == desc)
    {
      poly_field_get(&packet->mFields[i], (uint8_t*)data);
      ret = packet->mFields[i].mSize;
    }
  }

  MRT_MUTEX_UNLOCK(packet->mMutex);
  return ret;
}

int poly_packet_set_field(poly_packet_t* packet, const poly_field_desc_t* desc,const void* data)
{
  MRT_MUTEX_LOCK(packet->mMutex);
  int ret =0;
  for(int i=0; i < packet->mDesc->mFieldCount; i++ )
  {
    if(packet->mDesc->mFields[i] == desc)
    {
      poly_field_set(&packet->mFields[i], (const uint8_t*)data);
      ret = 1;
    }
  }
  MRT_MUTEX_UNLOCK(packet->mMutex);
  return ret;
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


ParseStatus_e poly_packet_parse_buffer(poly_packet_t* packet, const uint8_t* data, int len)
{
  int idx=0;                //cursor in data
  int expectedLen =0;       //length indicated in header
  uint32_t currFieldId;

  const poly_packet_desc_t* desc = packet->mDesc;


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
      //allow packets to parse if too much data is given, this is useful for pulling structs out of memory
      len = expectedLen;
  }


  //mark all fields as preset/absent
  for(int i=0; i < desc->mFieldCount; i++)
  {
    idx += poly_var_size_read(&data[idx],&currFieldId);

    if(currFieldId < desc->mFieldCount)
    {
      packet->mFields[currFieldId].mPresent = true;
      idx+= poly_field_parse(&packet->mFields[currFieldId], &data[idx]);
      if(idx > len)
      {
        return PACKET_PARSING_ERROR;
      }
    }
    else
    {
      return PACKET_PARSING_ERROR;
    }

    if(idx== len)
    {
      break;
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

ParseStatus_e poly_packet_parse_json_obj(poly_packet_t* packet, json_obj_t* json)
{
  //iterate through text and parse key/value pairs
  for(int i=0 ; i < json->mAttributeCount; i++)
  {

    for(int a=0; a < packet->mDesc->mFieldCount; a++)
    {
      if((!packet->mFields[a].mPresent) && (strcmp(packet->mFields[a].mDesc->mName, json->mAttributes[i].mKey) == 0))
      {
        poly_field_parse_str(&packet->mFields[a],json->mAttributes[i].mVal);
        break;
      }
    }

  }

  return PACKET_VALID;

}


int poly_packet_pack(poly_packet_t* packet, uint8_t* data)
{
  int idx=0;
  poly_field_t* field;
  packet->mHeader.mCheckSum = CHECKSUM_SEED;

  //Skip header for now, this will be written in after we get length and checksum
  idx+=sizeof(poly_packet_hdr_t);

  int i=0;
  //copy fields
  for(i=0; i < packet->mDesc->mFieldCount; i++)
  {
    field = &packet->mFields[i];
    if(field->mPresent)
    {
      // insert field id
      idx+= poly_var_size_pack(i, &data[idx]);

      //if its a variable length field, put int length first (max 255)
      if(field->mDesc->mVarLen)
      {
        idx+= poly_var_size_pack(field->mSize, &data[idx]);
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

int poly_packet_pack_encoded(poly_packet_t* packet, uint8_t* data)
{
	int decodedSize = poly_packet_max_packed_size(packet);
  uint8_t decoded[decodedSize];
  int decodedLen = poly_packet_pack(packet, decoded);

  return cobs_encode(decoded, decodedLen, data);
}


int poly_packet_copy(poly_packet_t* dst, poly_packet_t* src)
{
  int count =0;
  for(int i=0; i< src->mDesc->mFieldCount; i++)
  {
    for(int a=0; a< dst->mDesc->mFieldCount; a++)
    {
      //poly_field_copy makes sure the fields are compatible
      count += poly_field_copy(&dst->mFields[a], &src->mFields[i]);
    }
  }

  return count;
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

  idx += MRT_SPRINTF(&buf[idx]," \"packetType\" : \"%s\" ", packet->mDesc->mName);

  if(printHeader)
  {
    idx += MRT_SPRINTF(&buf[idx],", \"typeId\" : \"%02X\" ,", packet->mHeader.mTypeId);
    idx += MRT_SPRINTF(&buf[idx]," \"token\" : \"\%04X\" ,", packet->mHeader.mToken);
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

int poly_packet_print_packed(poly_packet_t* packet, char* buf)
{
  uint8_t data[packet->mDesc->mMaxPacketSize];
  int strLen = 0;

  //pack data
  int len = poly_packet_pack(packet, data);

  //print it to buffer
  for(int i=0; i < len; i++)
  {
    strLen+= MRT_SPRINTF(&buf[strLen], " %02X", data[i]);
  }

  return strLen;
}

int poly_packet_update_header(poly_packet_t* packet)
{

  uint8_t data[packet->mDesc->mMaxPacketSize];

  //easiest way to get the meta data is to pack the message
  //Its not very effecient but this should be a rare use-case, mainly for debugging
  int len = poly_packet_pack(packet, data);

  return len;
}

int poly_packet_max_packed_size(poly_packet_t* packet)
{
  int len = sizeof(poly_packet_hdr_t);
	poly_field_t* field;

  for(int i=0; i < packet->mDesc->mFieldCount; i++)
  {
    field = &packet->mFields[i];

    if(field->mPresent)
    {
      len+= field->mSize;
      len+= 4; //for field header and size
    }

  }

  return len;

}
