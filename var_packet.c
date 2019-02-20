/**
  *@file var_packet.c
  *@brief implementation for variable packet module
  *@author Jason Berger
  *@date 02/19/"2019
  */

#include "var_packet.h"

#define DYNAMIC_GROWTH_SIZE 16

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

  packet->mData = NULL;
  packet->mLen =0;


}

void packet_destroy(var_packet_t* packet)
{
  for(int i=0; i < packet->mDesc->mFieldCount; i++)
  {
    destroy_field(packet->mFields[i]);
  }
  free(packet->mFields);
  free(packet->mData);
  free(packet);
}

void packet_reset(var_packet_t* packet)
{
  for(int i=0; i < packet->mDesc->mFieldCount; i++)
  {
    destroy_field(packet->mFields[i]);
  }
  free(packet->mFields);
  free(packet->mData);
  packet->mDesc = NULL;
}



var_field_t* packet_field(var_packet_t* packet, field_desc_t* fieldDesc)
{
  for(int i=0; i < packet->mFieldCount; i++)
  {
    if(packet->mFields[i]->mDesc == fieldDesc)
    {
      return &packet->mFields[i];
    }
  }
}

ePacketStatus packet_parse(var_packet_t* packet, uint8_t* data, int len )
{



}
