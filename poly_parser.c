/**
  *@file poly_parser.c
  *@brief implementation for variable packet module
  *@author Jason Berger
  *@date 02/19/"2019
  */

#include "poly_parser.h"
#include <assert.h>



poly_parser_t* new_poly_parser(int maxDescs, int interfaceCount)
{
  //allocate parser
  poly_parser_t* parser = (poly_parser_t*)malloc(sizeof(poly_parser_t));

  //allocate interfaces
  parser->mInterfaceCount = interfaceCount;
  parser->mInterfaces = (poly_interface_t*) malloc(sizeof(poly_interface_t) * interfaceCount);

  //allocate packet descriptors
  parser->mMaxDescs = maxDescs;
  parser->mPacketDescs = (poly_packet_desc_t**) malloc(sizeof(poly_packet_desc_t*));

  parser->mDescCount =0;
  parser->mStarted = false;

  return parser;
}


void poly_parser_register_desc(poly_parser_t* pParser, poly_packet_desc_t* pDesc)
{
  assert(pParser->mDescCount < pParser->mMaxDescs);

  pParser->mDescs[pParser->mDescCount++] = pDesc;
}


void poly_parser_start(poly_parser_t* pParser, int fifoDepth)
{

  //find max packet size
  for(int i=0; i < pParser->mDescCount; i++)
  {
    pParser->mMaxPacketSize = max(pParser->mMaxPacketSize, mParser->mDescs[i]->mMaxPacketSize);
  }

  //initialize interfaces
  for(int i=0; i < pParser->mInterfaceCount;i++)
  {
    //reset diagnostic info
    pParser->mInterfaces[i]->mPacketsIn = 0;
    pParser->mInterfaces[i]->mPacketsOut = 0;
    pParser->mInterfaces[i]->mRetries = 0;
    pParser->mInterfaces[i]->mFailures = 0;
    pParser->mInterfaces[i]->mBitErrors = 0;

    //set up buffers, make incoming byte buffer 2x max packet size
    pParser->mInterfaces[i]->mParseState= STATE_WAITING_FOR_HEADER;
    pParser->mInterfaces[i]->mRaw = (uint8_t*) malloc(pParser->mMaxPacketSize);

    fifo_init(&pParser->mInterfaces[i]->mPacketFifo, fifoDepth, sizeof(poly_packet_t));
  }

  pParser->mStarted = true;

}



void poly_parser_feed(poly_parser_t* pParser, int interface, uint8_t* data, int len)
{
  poly_interface_t* iface = pParser->mInterfaces[i];

  //make sure we stay in bounds
  len = min(len, (pParser->mMaxPacketSize - iface->mIdx));


}


bool poly_parser_next(poly_parser_t* pParser, int interface,  poly_packet_t* pPacket)
{
  if(pParser->mInterfaces[interface]->mPacketFifo->mCount == 0)
    return false;

  fifo_pop(&pParser->mInterfaces[interface]->mPacketFifo, pPacket);
  return true;
}

bool poly_parser_seek_header(poly_parser_t* pParser, poly_interface_t* iface)
{
  uint8_t trash;
  uint8_t id;
  bool retVal =false;
  //if we dont have at least the size of a header, it cant be valid
  while(!(iface->mBytefifo.mCount < sizeof(poly_packet_hdr_t)))
  {
    //peek in next header
    fifo_peek_buf(&iface->mBytefifo, &iface->mCurrentHdr, sizeof(poly_packet_hdr_t));

    id = iface->mCurrentHdr.mTypeId
    //if packet type is valid, and length is valid for that packet type, we have a candidate
    if((iface->mCurrentHdr.mTypeId < pParser->mDescCount) && (iface->mCurrentHdr.mDataLen < pParser->mDescs[iface->mCurrentHdr.mTypeId]->mMaxPacketSize))
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

ePacketStatus poly_parser_try_parse(poly_parser_t* pParser, poly_packet_t* packet, int interface)
{
  poly_interface_t* iface = &pParser->mInterfaces[interface];
  ePacketStatus retVal = PACKET_NONE;
  uint16_t checksumComp;
  uint8_t trash;
  int len;

  //if we dont have at least the size of a header, it cant be valid
  while((iface->mBytefifo.mCount >= sizeof(poly_packet_hdr_t))&& (retVal == PACKET_NONE) )
  {
    if(iface->mParseState->mParseState == STATE_WAITING_FOR_HEADER)
    {
        //moves tail of fifo to next possible header
        poly_parser_seek_header(pParser, iface);
    }

    if(iface->mParseState->mParseState == STATE_HEADER_FOUND)
    {
        len = iface->mCurrentHdr.mDataLen + PACKET_METADATA_SIZE;
        //see if fifo has enough to contain the entire packet
        if(iface->mBytefifo.mCount >= len)
        {
          //calculate checksum of data in the fifo, if it matches the checksum in the header, we have a valid packet
          checksumComp =fifo_checksum(&iface->mBytefifo, sizeof(poly_packet_hdr_t), iface->mCurrentHdr.mDataLen );
          if(checksumComp == iface->mCurrentHdr.mCheckSum)
          {
            //Valid packet, Parse!
            fifo_peek_buf(&iface->mBytefifo, pParser->mRaw, len );
            newPacket = new_poly_packet(pParser->mDescs[iface->mCurrentHdr.mTypeId], true);

            //push new packet to the packet fifo (makes a copy)
            fifo_push(&iface->mPacketFifo, newPacket);

            //free up memory since we made a copy..
            free(newPacket);
            retVal = PACKET_VALID;
          }
          else
          {
            //bad checksum throw away leading byte and try again
            iface->mParseState->mParseState == STATE_WAITING_FOR_HEADER;

          }
        }
    }
  }

  return retVal;

}







#ifdef __cplusplus
}
#endif
