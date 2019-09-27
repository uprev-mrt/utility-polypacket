/**
  *@file poly_service.c
  *@brief implementation for variable packet module
  *@author Jason Berger
  *@date 02/19/"2019
  */



#include "poly_service.h"
#include <assert.h>

#if defined(POLY_PACKET_DEBUG_LVL)
extern char POLY_DEBUG_PRINTBUF[512];
#endif

void poly_service_init( poly_service_t* service, int maxDescs, int interfaceCount)
{

  //allocate interfaces
  service->mInterfaceCount = interfaceCount;
  service->mInterfaces = (poly_interface_t*) malloc(sizeof(poly_interface_t) * interfaceCount);

  //allocate packet descriptors
  service->mMaxDescs = maxDescs;
  service->mPacketDescs = (poly_packet_desc_t**) malloc(maxDescs * sizeof(poly_packet_desc_t*));

  service->mDescCount =0;
  service->mStarted = false;
  service->mAutoAck = true;
}

void poly_service_deinit(poly_service_t* service)
{
  //deinitialize interfaces
  for(int i=0; i < service->mInterfaceCount;i++)
  {

    cobs_fifo_deinit(&service->mInterfaces[i].mBytefifo);

    poly_spool_deinit(&service->mInterfaces[i].mOutSpool);
  }

  free(service->mInterfaces);
  free(service->mPacketDescs); /* this causes an invalid free. this is an unlikely use case but should be investigater */
}


void poly_service_register_desc(poly_service_t* pService, poly_packet_desc_t* pDesc)
{
  assert(pService->mDescCount < pService->mMaxDescs);

  #if defined(POLY_PACKET_DEBUG_LVL) && POLY_PACKET_DEBUG_LVL > 1
  printf("Packet Descriptor: %s [%d]  fieldCount: %d (%d optional) , manifestSize: %d , mMaxPacketSize: %d\n", pDesc->mName,pService->mDescCount, pDesc->mFieldCount,pDesc->mOptionalFieldCount, pDesc->mManifestSize, pDesc->mMaxPacketSize );
  #endif

  pService->mPacketDescs[pService->mDescCount++] = pDesc;
}

void poly_service_register_bytes_tx_callback(poly_service_t* pService, int interface, poly_tx_bytes_callback callback)
{
  assert(interface < pService->mInterfaceCount);

  pService->mInterfaces[interface].f_TxBytes = callback;
}

void poly_service_register_packet_tx_callback(poly_service_t* pService, int interface, poly_tx_packet_callback callback)
{
  assert(interface < pService->mInterfaceCount);

  pService->mInterfaces[interface].f_TxPacket = callback;
}


void poly_service_start(poly_service_t* pService, int depth)
{
  int fifo_depth;
  //find max packet size
  for(int i=0; i < pService->mDescCount; i++)
  {
    if(pService->mPacketDescs[i]->mMaxPacketSize > pService->mMaxPacketSize)
      pService->mMaxPacketSize = pService->mPacketDescs[i]->mMaxPacketSize;
  }

  //ensure fifo is on 8byte alignment
  fifo_depth = (depth * pService->mMaxPacketSize)/8;
  fifo_depth = fifo_depth *8;

  //initialize interfaces
  for(int i=0; i < pService->mInterfaceCount;i++)
  {
    //reset diagnostic info
    pService->mInterfaces[i].mPacketsIn = 0;
    pService->mInterfaces[i].mPacketsOut = 0;
    pService->mInterfaces[i].f_TxBytes = NULL;
    pService->mInterfaces[i].f_TxPacket = NULL;
    pService->mInterfaces[i].mTxReady = true;

    //set up buffers for incoming data
    cobs_fifo_init(&pService->mInterfaces[i].mBytefifo, depth * pService->mMaxPacketSize );

    //set up spool for outgoing
    poly_spool_init(&pService->mInterfaces[i].mOutSpool, depth);
  }

  pService->mStarted = true;

}



void poly_service_feed(poly_service_t* pService, int interface, const uint8_t* data, int len)
{
  assert(interface < pService->mInterfaceCount);

  poly_interface_t* iface = &pService->mInterfaces[interface];


  cobs_fifo_push_buf(&iface->mBytefifo, data, len);
}

void poly_service_feed_json_msg(poly_service_t* pService, int interface,const char* msg, int len)
{
  //parse json to packet
  poly_packet_t packet;
  uint8_t tmp[pService->mMaxPacketSize];
  int packedLen=0;

  poly_service_parse_json(pService, &packet, msg, len);


  //pack packet
  packedLen = poly_packet_pack_encoded(&packet, tmp);

  //feed packet to service
  poly_service_feed(pService, interface, tmp, packedLen);

  //destroy packet
  poly_packet_clean(&packet);

}

ParseStatus_e poly_service_parse_json(poly_service_t* pService, poly_packet_t* packet ,const char* msg, int len)
{
  //parse json to packet
  json_obj_t json;
  int typeId = -1;
  uint8_t tmp[pService->mMaxPacketSize];
  int packedLen=0;


  json_parse_string(&json, msg, len);


  //get type
  for(int i=0; i < json.mAttributeCount; i++)
  {
    if(strcmp("packetType",json.mAttributes[i].mKey) == 0)
    {
      for(int a=0; a < pService->mDescCount; a++)
      {
        if(strcmp(pService->mPacketDescs[a]->mName,json.mAttributes[i].mVal) == 0)
        {
          typeId = a;
          break;
        }
      }
      break;
    }
  }


  if(typeId > -1)
  {
      poly_packet_build(packet, pService->mPacketDescs[typeId],true);

      poly_packet_parse_json_obj(packet, &json);

      #if defined(POLY_PACKET_DEBUG_LVL) && POLY_PACKET_DEBUG_LVL >0
        //If debug is enabled, print json of outgoing packets
        #if POLY_PACKET_DEBUG_LVL == 4
        poly_packet_print_json(&packet, POLY_DEBUG_PRINTBUF, true );
        printf("  JSON PARSED:  %s\n\n",POLY_DEBUG_PRINTBUF );
        #endif
      #endif

      return PACKET_VALID;
  }

  json_clean(&json);

  return PACKET_PARSING_ERROR;
}



ParseStatus_e poly_service_try_parse_interface(poly_service_t* pService, poly_packet_t* packet, poly_interface_t* iface)
{
  ParseStatus_e retVal = PACKET_NONE;
  uint16_t checksumComp;
  int encodedLen = cobs_fifo_get_next_len(&iface->mBytefifo);  //gets the length of the encoded frame which is always longer than decoded
  int decodedLen;
	uint8_t* frame;
  if(encodedLen > 0)
  {

		frame = malloc(encodedLen);
    decodedLen = cobs_fifo_pop_frame(&iface->mBytefifo,frame,encodedLen);



    if(decodedLen >= sizeof(poly_packet_hdr_t))
    {
      memcpy((uint8_t*)&iface->mCurrentHdr, frame, sizeof(poly_packet_hdr_t));

			if(iface->mCurrentHdr.mTypeId < pService->mDescCount)
			{
		     poly_packet_build(packet, pService->mPacketDescs[iface->mCurrentHdr.mTypeId], true);

		      retVal = poly_packet_parse_buffer(packet, frame, decodedLen );

		      //if the parse failed, clean the packet
		      if(retVal != PACKET_VALID)
          {
            poly_packet_clean(packet);
          }
			}
      else
      {
        retVal = PACKET_PARSING_ERROR;
      }

    }

		free(frame);

  }


  return retVal;

}

ParseStatus_e poly_service_try_parse(poly_service_t* pService, poly_packet_t* packet)
{
  ParseStatus_e retVal = PACKET_NONE;
  for(int i=0; i < pService->mInterfaceCount; i++)
  {
    retVal = poly_service_try_parse_interface(pService, packet, &pService->mInterfaces[i]);
    if (retVal == PACKET_VALID)
    {
      retVal=  PACKET_VALID;
      packet->mInterface = i;
      #if defined(POLY_PACKET_DEBUG_LVL) && POLY_PACKET_DEBUG_LVL >0
        //If debug is enabled, print json of outgoing packets
        #if POLY_PACKET_DEBUG_LVL == 1
        poly_packet_print_json(packet, POLY_DEBUG_PRINTBUF, false );
        printf("\n\033[1;33mIN <<< %s\n",POLY_DEBUG_PRINTBUF );
        #elif POLY_PACKET_DEBUG_LVL > 1
        poly_packet_print_json(packet, POLY_DEBUG_PRINTBUF, true );
        printf("  IN <<< %s\n\n",POLY_DEBUG_PRINTBUF);
        #endif
        #if POLY_PACKET_DEBUG_LVL > 2
        poly_packet_print_packed(packet, POLY_DEBUG_PRINTBUF);
        printf("  IN <<< %s\n\n", POLY_DEBUG_PRINTBUF );
        #endif
      #endif
      break;
    }

  }

  return retVal;
}

HandlerStatus_e poly_service_spool(poly_service_t* pService, int interface,  poly_packet_t* packet)
{
   HandlerStatus_e status = PACKET_NOT_HANDLED;
   assert(interface < pService->mInterfaceCount);

   poly_interface_t* iface = &pService->mInterfaces[interface];


   if((iface->f_TxBytes == NULL) && (iface->f_TxPacket == NULL))
   {
     return PACKET_NOT_HANDLED; /* no tx callback registered */
   }

   if(poly_spool_push(&iface->mOutSpool, packet) != SPOOL_OK)
   {
     return PACKET_NOT_HANDLED;
   }

  return PACKET_SPOOLED;
}

/**
  *@brief grabs next available packet from interface
  *@param interface ptr to interface
  *@param data ptr to store data
  */
bool poly_service_despool_interface(poly_interface_t* iface, poly_packet_t* packet)
{
  bool ret = false;
  int len;
  int packedLen;
  uint8_t* encoded;

  if(( iface->mTxReady ) && (iface->mOutSpool.mReadyCount > 0))
  {
    if(poly_spool_pop(&iface->mOutSpool, packet) == SPOOL_OK)
    {
      ret = true;
      packedLen = poly_packet_max_packed_size(packet);

      encoded = malloc(COBS_MAX_LEN(packedLen));

      len = poly_packet_pack_encoded(packet, encoded);

      if(iface->f_TxBytes)
        iface->f_TxBytes(encoded,len);
      else if(iface->f_TxPacket)
        iface->f_TxPacket(&packet);

      free(encoded);

      #if defined(POLY_PACKET_DEBUG_LVL) && POLY_PACKET_DEBUG_LVL >0
        //If debug is enabled, print json of outgoing packets
        #if POLY_PACKET_DEBUG_LVL == 1
        poly_packet_print_json(&packet, POLY_DEBUG_PRINTBUF, false );
        printf("\n\033[1;34mOUT >>> %s\n",POLY_DEBUG_PRINTBUF );
        #elif POLY_PACKET_DEBUG_LVL > 1
        poly_packet_print_json(&packet, POLY_DEBUG_PRINTBUF, true );
        printf(" OUT >>> %s\n",POLY_DEBUG_PRINTBUF );
        #endif
        #if POLY_PACKET_DEBUG_LVL > 2
        poly_packet_print_packed(&packet, POLY_DEBUG_PRINTBUF);
        printf(" OUT >>> %s\n\n", POLY_DEBUG_PRINTBUF );
        #endif
      #endif

    }
  }

  return ret;

}


bool poly_service_despool(poly_service_t* pService)
{
  bool ret = false;

  poly_packet_t outPacket;

  for(int i=0; i < pService->mInterfaceCount; i++)
  {
    if(poly_service_despool_interface(&pService->mInterfaces[i], &outPacket))
    {
      ret = true;
    }
  }

  return ret;
}

void poly_service_tick(poly_service_t* pService, uint32_t ms)
{
  for(int i=0; i < pService->mInterfaceCount; i++)
  {
    poly_spool_tick(&pService->mInterfaces[i].mOutSpool, ms);
  }
}
