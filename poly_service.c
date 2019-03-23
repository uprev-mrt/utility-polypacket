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
  service->mPacketDescs = (poly_packet_desc_t**) malloc(sizeof(poly_packet_desc_t*));

  service->mDescCount =0;
  service->mStarted = false;
  service->mAutoAck = true;
}

void poly_service_deinit(poly_service_t* service)
{
  //deinitialize interfaces
  for(int i=0; i < service->mInterfaceCount;i++)
  {
    //set up buffers for incoming data
    fifo_deinit(&service->mInterfaces[i].mBytefifo);

    free(service->mInterfaces[i].mRaw);

    //set up spool for outgoing
    poly_spool_deinit(&service->mInterfaces[i].mOutSpool);
  }

  free(service->mInterfaces);
  //free(service->mPacketDescs); /* this causes an invalid free. this is an unlikely use case but should be investigater */
}


void poly_service_register_desc(poly_service_t* pService, poly_packet_desc_t* pDesc)
{
  assert(pService->mDescCount < pService->mMaxDescs);

  #if defined(POLY_PACKET_DEBUG_LVL) && POLY_PACKET_DEBUG_LVL > 1
  printf("Packet Descriptor: %s  fieldCount: %d (%d optional) , manifestSize: %d , mMaxPacketSize: %d\n", pDesc->mName, pDesc->mFieldCount,pDesc->mOptionalFieldCount, pDesc->mManifestSize, pDesc->mMaxPacketSize );
  #endif

  pService->mPacketDescs[pService->mDescCount++] = pDesc;
}

void poly_service_register_tx_callback(poly_service_t* pService, int interface, poly_tx_callback callback)
{
  assert(interface < pService->mInterfaceCount);

  pService->mInterfaces[interface].f_TxCallBack = callback;
}


void poly_service_start(poly_service_t* pService, int depth)
{

  //find max packet size
  for(int i=0; i < pService->mDescCount; i++)
  {
    if(pService->mPacketDescs[i]->mMaxPacketSize > pService->mMaxPacketSize)
      pService->mMaxPacketSize = pService->mPacketDescs[i]->mMaxPacketSize;
  }

  //initialize interfaces
  for(int i=0; i < pService->mInterfaceCount;i++)
  {
    //reset diagnostic info
    pService->mInterfaces[i].mPacketsIn = 0;
    pService->mInterfaces[i].mPacketsOut = 0;
    pService->mInterfaces[i].mRetries = 0;
    pService->mInterfaces[i].mFailures = 0;
    pService->mInterfaces[i].mBitErrors = 0;
    pService->mInterfaces[i].f_TxCallBack = NULL;

    //set up buffers for incoming data
    fifo_init(&pService->mInterfaces[i].mBytefifo, depth, sizeof(uint8_t));
    pService->mInterfaces[i].mParseState= STATE_WAITING_FOR_HEADER;
    pService->mInterfaces[i].mRaw = (uint8_t*) malloc(pService->mMaxPacketSize);

    //set up spool for outgoing
    poly_spool_init(&pService->mInterfaces[i].mOutSpool, depth);
  }

  pService->mStarted = true;

}



void poly_service_feed(poly_service_t* pService, int interface, uint8_t* data, int len)
{
  assert(interface < pService->mInterfaceCount);

  poly_interface_t* iface = &pService->mInterfaces[interface];

  fifo_push_buf(&iface->mBytefifo, data, len);
}

bool poly_service_seek_header(poly_service_t* pService, poly_interface_t* iface)
{
  uint8_t trash;
  uint8_t id;
  bool retVal =false;
  //if we dont have at least the size of a header, it cant be valid
  while(!(iface->mBytefifo.mCount < sizeof(poly_packet_hdr_t)))
  {
    //peek in next header
    fifo_peek_buf(&iface->mBytefifo, &iface->mCurrentHdr, sizeof(poly_packet_hdr_t));

    id = iface->mCurrentHdr.mTypeId;

    //if packet type is valid, and length is valid for that packet type, we have a candidate
    if((iface->mCurrentHdr.mTypeId < pService->mDescCount) && (iface->mCurrentHdr.mDataLen < pService->mPacketDescs[iface->mCurrentHdr.mTypeId]->mMaxPacketSize))
    {
      iface->mParseState = STATE_HEADER_FOUND;
      retVal = true;
      break;
    }
    else
    {
      //if we are not on a valid header, throw away the fifo tail and move on to the next byte
      fifo_pop(&iface->mBytefifo, &trash);
    }

  }
    return retVal;
}

ParseStatus_e poly_service_try_parse_interface(poly_service_t* pService, poly_packet_t* packet, int interface)
{
  poly_interface_t* iface = &pService->mInterfaces[interface];
  ParseStatus_e retVal = PACKET_NONE;
  uint16_t checksumComp;
  uint8_t trash;
  int len;

  //if we dont have at least the size of a header, it cant be valid
  while((iface->mBytefifo.mCount >= sizeof(poly_packet_hdr_t))&& (retVal == PACKET_NONE) )
  {
    if(iface->mParseState == STATE_WAITING_FOR_HEADER)
    {
        //moves tail of fifo to next possible header
        poly_service_seek_header(pService, iface);
    }

    if(iface->mParseState == STATE_HEADER_FOUND)
    {
        len = iface->mCurrentHdr.mDataLen + PACKET_METADATA_SIZE;
        //see if fifo has enough to contain the entire packet
        if(iface->mBytefifo.mCount >= len)
        {
          //calculate checksum of data in the fifo, if it matches the checksum in the header, we have a valid packet
          checksumComp = CHECKSUM_SEED + fifo_checksum(&iface->mBytefifo, sizeof(poly_packet_hdr_t), iface->mCurrentHdr.mDataLen );
          if(checksumComp == iface->mCurrentHdr.mCheckSum)
          {
            //Valid packet, Parse!
            fifo_peek_buf(&iface->mBytefifo, iface->mRaw, len );

            poly_packet_build(packet, pService->mPacketDescs[iface->mCurrentHdr.mTypeId] ,true);

            packet->mInterface = interface;

            retVal = poly_packet_parse_buffer(packet, iface->mRaw, len);

            switch(retVal)
            {
              case PACKET_VALID:
                fifo_clear(&iface->mBytefifo, len); //remove these bytes from fifo
                iface->mPacketsIn++;
                break;
              default:
                fifo_clear(&iface->mBytefifo, 1); //remove one byte
                poly_packet_clean(packet);
                break;
            }
          }
          else
          {
            //bad checksum throw away leading byte and try again
            iface->mParseState = STATE_WAITING_FOR_HEADER;
          }
        }
    }
  }

  return retVal;

}

ParseStatus_e poly_service_try_parse(poly_service_t* pService, poly_packet_t* packet)
{
  ParseStatus_e retVal = PACKET_NONE;
  for(int i=0; i < pService->mInterfaceCount; i++)
  {
    if (poly_service_try_parse_interface(pService, packet,i) == PACKET_VALID)
    {
      retVal=  PACKET_VALID;
      break;
    }
  }

  return retVal;
}

ParseStatus_e poly_service_spool(poly_service_t* pService, int interface,  poly_packet_t* packet)
{
   ParseStatus_e status = PACKET_NOT_HANDLED;
   assert(interface < pService->mInterfaceCount);

   poly_interface_t* iface = &pService->mInterfaces[interface];


   if(iface->f_TxCallBack == NULL)
   {
     return PACKET_NOT_HANDLED; /* no tx callback registered */
   }

   if(poly_spool_push(&iface->mOutSpool, packet) != SPOOL_OK)
   {
     return PACKET_NOT_HANDLED;
   }

  return PACKET_SPOOLED;
}

ParseStatus_e poly_service_despool(poly_service_t* pService)
{
  ParseStatus_e status = PACKET_NOT_HANDLED;
  poly_interface_t* iface;
  poly_packet_t outPacket;
  int len;

  for(int i=0; i < pService->mInterfaceCount; i++)
  {
    iface = &pService->mInterfaces[i];
    if(iface->mOutSpool.mReadyCount > 0)
    {
      if(poly_spool_pop(&iface->mOutSpool, &outPacket) == SPOOL_OK)
      {
        uint8_t data[outPacket.mDesc->mMaxPacketSize];
        len = poly_packet_pack(&outPacket, data);

        #if defined(POLY_PACKET_DEBUG_LVL)
          //If debug is enabled, print json of outgoing packets
          #if POLY_PACKET_DEBUG_LVL == 1
          poly_packet_print_json(&outPacket, POLY_DEBUG_PRINTBUF, false );
          printf(" OUT >>> %s\n",POLY_DEBUG_PRINTBUF );
          #elif POLY_PACKET_DEBUG_LVL > 1
          poly_packet_print_json(&outPacket, POLY_DEBUG_PRINTBUF, true );
          printf(" OUT >>> %s\n",POLY_DEBUG_PRINTBUF );
          #endif
          #if POLY_PACKET_DEBUG_LVL > 2
          poly_packet_print_packed(&outPacket, POLY_DEBUG_PRINTBUF);
          printf(" OUT >>> %s\n\n", POLY_DEBUG_PRINTBUF );
          #endif
        #endif

        status = iface->f_TxCallBack(data,len);
        break;
      }
    }

  }

  return status;
}
