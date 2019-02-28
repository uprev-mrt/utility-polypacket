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
    pParser->mInterfaces[i]->mRawMessage = (uint8_t*) malloc(pParser->mMaxPacketSize);
    pParser->mInterfaces[i]->mIdx = 0;
    pParser->mInterfaces[i]->mExpectedPacketLen = -1;
    pParser->mInterfaces[i]->mValidHeader = false;

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

bool poly_parser_seek_valid_header(poly_parser_t* pParser, poly_interface_t* iface)
{
  poly_packet_hdr_t* pHdr;
  int hdrAddr =-1;
  int i;
  //we must have atleast the header size to be a valid header
  if(iface->mIdx < sizeof(poly_packet_hdr_t))
    return false;

  for( i=0; i < (iface->mIdx - sizeof(poly_packet_hdr_t)) ; i++)
  {
    pHdr = (poly_packet_hdr_t*) &iface->mByteBuffer[i];

    //check if typeid is valid and data len is valid for that packet type
    if((pHdr->mTypeId < pParser->mDescCount) && (pHdr->mDataLen <= pParser->mDescs[pHdr->mTypeId]->mMaxPacketSize))
    {
      hdrAddr = i;
      break;
    }
  }

  //whether we found a header or not, we can get rid of any bytes before i

  if()
  memcpy(iface->mByteBuffer, iface->mByteBuffer[i], iface->mIdx -i);
  iface->mIdx -= i; // subtract from byte count


  //if no valid headers are in data, we have missed the beginning of a packet and need to clear it
  if(hdrAddr == -1)
  {
    i++; // move past the last idx we checked because we know it isnt a valid packet

    //copy remaining bytes to beginning of buffer;
    memcpy(iface->mByteBuffer, iface->mByteBuffer[i], iface->mIdx -i);
    iface->mIdx -= i; // subtract from byte count

    return false;
  }

  //if we did find a possible header




}







#ifdef __cplusplus
}
#endif
