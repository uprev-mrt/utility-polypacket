/**
  *@file var_packet.c
  *@brief implementation for variable packet module
  *@author Jason Berger
  *@date 02/19/"2019
  */

#include "var_packet.h"


#define PACKET_METADATA_SIZE (sizeof(var_packet_hdr_t) + sizeof(var_packet_ftr_t))
#define DYNAMIC_GROWTH_SIZE 8

/**
  *@brief Parsing dictionary. used to index packet descriptors and parse messages
  */
typedef struct{
  packet_desc_t** mEntries;
  int mMaxEntries;
  int mEntryCount;
}parsing_dictionary_t;

parsing_dictionary_t MPD; //Master Parsing Dictionary



void parsing_dictionary_init()
{
  MPD.mMaxEntries = DYNAMIC_GROWTH_SIZE;
  MPD.mEntryCount =0;

  MPD.mEntries = (packet_desc_t**) malloc(sizeof(packet_desc_t*) * MPD.mMaxEntries);
}

packet_desc_t* new_packet_desc(const char* name)
{
  static int id =0;

  //on first creation, initialize parsing dictionary
  if(id == 0)
  {
    parsing_dictionary_init();
  }

  //if parsing dictionary is full, we re-allocate and copy to larger memory block
  if(MPD.mEntryCount == MPD.mMaxEntries)
  {
    //increase max size
    MPD.mMaxEntries+=DYNAMIC_GROWTH_SIZE;

    //allocate new, larger block
    packet_desc_t** newEntries = (packet_desc_t**) malloc(sizeof(packet_desc_t*) * MPD.mMaxEntries);

    //copy old fields and then free their memory
    memcpy((void*)newEntries, (void*)MPD.mEntries, MPD.mEntryCount );
    free(MPD.mEntries);
    MPD.mEntries = newEntries;
  }

  //build out new packet descriptor
  packet_desc_t* desc = (packet_desc_t*)malloc(sizeof(packet_desc_t));
  desc->mPayloadId = id++;
  desc->mName = name;
  desc->mMaxFields = DYNAMIC_GROWTH_SIZE;
  desc->mFieldCount =0;
  mFields = (field_desc_t*) malloc(sizeof(field_desc_t*) * desc->mMaxFields);

  //add packet descriptor to master parsing dictionary
  MPD.mEntries[MPD.mEntryCount++] = desc;

  return desc;
}

void packet_desc_add_field(packet_desc_t* desc, field_desc_t* fieldDesc)
{
  // if we are already at max size, re-allocate.
  if(desc->mFieldCount == desc->mMaxFields)
  {
    //increase max size
    desc->mMaxFields+=DYNAMIC_GROWTH_SIZE;

    //allocate new, larger block
    field_desc_t** newFields = (field_desc_t**) malloc(sizeof(field_desc_t*) * desc->mMaxFields);

    //copy old fields and then free their memory
    memcpy((void*)newFields, (void*)desc->mFields, desc->mFieldCount );
    free(desc->mFields);
    desc->mFields = newFields;
  }

  //add field desc to packet_desc
  desc->mFields[desc->mFieldCount++] = fieldDesc;

  desc->mManifestSize = (desc->mFieldCount +8)/8;
}


var_packet_t* new_packet(packet_desc_t* desc)
{
  //alocate memory for packet
  var_packet_t* packet = (var_packet_t*)malloc(sizeof(var_packet_t));

  //allocate memory for fields
  packet->mFields = (var_field_t*) malloc(sizeof(var_packet_t*) * desc->mFieldCount );

  //set descriptor ptr
  packet->mDesc = desc;

  //initialize fields
  for(int i=0; i < desc->mFieldCount; i++)
  {
    init_field(&packet->mFields[i], desc);
  }


}

void packet_destroy(var_packet_t* packet)
{
  for(int i=0; i < packet->mDesc->mFieldCount; i++)
  {
    destroy_field(packet->mFields[i]);
  }
  free(packet->mFields);
  free(packet);
}

void packet_reset(var_packet_t* packet)
{
  for(int i=0; i < packet->mDesc->mFieldCount; i++)
  {
    destroy_field(packet->mFields[i]);
  }
  free(packet->mFields);

  packet->mDesc = NULL;
}



var_field_t* packet_field(var_packet_t* packet, field_desc_t* fieldDesc)
{
  for(int i=0; i < packet->mDesc->mFieldCount; i++)
  {
    if(packet->mFields[i]->mDesc == fieldDesc)
    {
      return &packet->mFields[i];
    }
  }
}


ePacketStatus packet_parse(var_packet_t* packet, uint8_t* data, int len )
{

  packet_desc_t* pDesc = NULL;   //packet descriptor for packet type
  uint32_t cursor=0;            //cursor in data
  int expectedLen;              //expected length from header info

  int bitOffset =0;           //bitOffset used when checking manifest
  uint8_t manifestByte;        //current byte in manifest being scanned

  uint16_t checkSumComp =0;        //computated checksum

  //packet must be at least as long as the meta data required for each packet
  if(len < PACKET_METADATA_SIZE)
    return PACKET_INCOMPLETE;

  //reset packet
  packet_reset(packet);

  //copy over header information
  memcpy((void*)packet->mHdr, (void*)data, sizeof(var_packet_hdr_t));
  cursor += sizeof(var_packet_hdr_t);

  //get expected size of packet from header
  expectedLen = packet->mHdr.mDataLen + PACKET_METADATA_SIZE

  //if sizes do not match, there is an error
  if(len < expectedLen)
  {
    return PACKET_INCOMPLETE;
  }
  else if(len > expectedLen)
  {
      return PACKET_PARSING_ERROR;
  }


  //find matching packetID in master parsing dictionary
  for(int i=0; i < MPD.mEntryCount; i ++)
  {
    //find matching payload id
    if(MPS.mEntries[i]->mPayloadId == packet->mHdr.mPayloadId)
    {
      pDesc = MPS.mEntries[i];
      break;
    }
  }

  //if no matching payload is found return invalid packet type
  if(pDesc == NULL)
  {
    return INVALID_PACKET_TYPE;
  }

  //Parse packet data using descriptor
  packet->mFields = (var_field_t*) malloc(sizeof(var_packet_t*) * pDesc->mFieldCount );


  //initialize fields and check if they are present in manifest
  //Manifests are dynamicly sized by field count in the packet descriptor
  for(int i=0; i < pDesc->mFieldCount; i++)
  {
    if(bitOffset == 0)
    {
      manifestByte = data[cursor++];
    }
    //initialize field from descriptor
    init_field(&packet->mFields[i], pDesc);

    //check bit in manifest to see if it is present in this packet
    if((manifestByte >> bitOffset )  & 0x01)
    {
      packet->mFields[i].mPresent = true;
    }
    //advance bit offset
    bitOffset++;

    //If we finish a byte in manifest, keep going to the next byte
    if(bitOffset == 8)
    {
      bitOffset = 0;
    }
  }

  //Parse data fields from message data
  for(int i=0; i < pDesc->mFieldCount; i++)
  {
    //if field is variable len, the first byte is the length
    if(pDesc->mFields[i]->mVarLen)
    {
      packet->mFields[i].mSize = data[cursor++];
    }

    //parse out field and move cursor
    cursor += var_field_parse(&packet->mFields[i], data[cursor]);

    //make sure we dont go out of bounds
    if(cursor >= len)
    {
      return PACKET_PARSING_ERROR;
    }
  }

  //we should just have the footer left after this
  if(cursor != (len - sizeof(packet->mFooter))
  {
    return PACKET_PARSING_ERROR;
  }

  memcpy((void*)&packet->mFooter.mChecksum, (void*)&data[cursor], sizeof(packet->mFooter.mCheckSum) );
  cursor += sizeof(packet->mFooter.mCheckSum);

  // validate checksum
  for(int i=0; i < packet->mhdr.mDataLen; i++)
  {
    checkSumComp += data[sizeof(var_packet_hdr_t) +i];
  }

  if(packet->mFoot.mCheckSum != checkSumComp)
  {
    return PACKET_BAD_CHECKSUM;
  }

  return PACKET_VALID;
}
