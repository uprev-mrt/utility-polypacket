/**
  *@file poly_parser.h
  *@brief header for variable packet module
  *@author Jason Berger
  *@date 02/19/2019
  */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "fifo.h"
#include "spool.h"


#ifdef __cplusplus

extern "C"
{
#endif

typedef void (*poly_rx_callback)(poly_packet_t* packet);

typedef enum parseState {
  STATE_WAITING_FOR_HEADER,
  STATE_HEADER_FOUND,
}eParseState;

typdef struct{
  uint8_t* mRaw; //raw packet being parsed
  int mIdx;      //index of raw message

  fifo_t mBytefifo;       //fifo of incoming bytes
  poly_packet_hdr_t mCurrentHdr; //header for current message candidate
  eParseState mParseState;


  fifo_t mPacketBuffer; //outgoing packet buffer

  //diagnostic info
  int mPacketsIn;     //Total number of incoming packets parsed
  int mPacketsOut;    //Total packets sent on on spool
  int mRetries;       //total number of packet retries (no ack)
  int mFailures;      //total number of packets that failed (hit max retries )
  int mBitErrors;     //total number of bit errors (bad packet parse)
  poly_rx_callback mCallback;
}poly_interface_t;

/**
  *@brief packet handling parser
  */
typedef struct{
  int mInterfaceCount;
  poly_interface_t* mInterfaces;
  poly_packet_desc_t** mPacketDescs;
  int mDescCount;
  int mMaxPacketSize;
  int mMaxDescs;
  int mMaxPacketSize;
}poly_parser_t;

/**
  *@brief create new parser
  *@param maxDesc max number of Packet descriptors in protocol
  *@param interfaceCount number of interfaces to allocate
  *@return ptr to newly allocated parser
  */
poly_parser_t* new_poly_parser(int maxDescs, int interfaceCount);

/**
  *@brief register packet descriptor with parser
  *@param pParser ptr to poly_parser
  *@param pDesc packet descriptor to be registered
  *@post Once all packets descriptors are registered the parser can be started
  */
void poly_parser_register_desc(poly_parser_t* pParser, poly_packet_desc_t* pDesc);

/**
  *@brief registers a packet received callback for a given interface
  *@param pParser ptr to poly parser
  *@param interface index of interface to register callback with
  *@post callback callback function
  */
void poly_parser_register_rx_callback(poly_parser_t* pParser, int interface, poly_rx_callback* callback);

/**
  *@brief Starts the parser with a given number of interfaces
  *@param pParser ptr to parser
  *@param fifoDepth how many objects the parsed packet fifo should hold
  *@pre must register all descriptors first
  */
void poly_parser_start(poly_parser_t* pParser, int fifoDepth);


/**
  *@brief 'Feeds' the parser bytes to parse
  *@param pParser ptr to poly parser
  *@param interface index of interface to feed
  *@param data ptr to data being added
  *@param len number of bytes being fed to parser
  */
void poly_parser_feed(poly_parser_t* pParser, int interface, uint8_t* data, int len);

/**
  *@brief gets next packet from packet buffer
  *@param pParser ptr to poly parser
  *@param interface index of interface to read from
  *@param pPacket ptr to store packet
  *@return True if packet was available
  *@return False if no packet is available
  */
bool poly_parser_next(poly_parser_t* pParser,int interface,  poly_packet_t* pPacket);


/**
  *@brief advances the idx in the interface buffer until the next valid header
  *@param pParser ptr to poly parser
  *@param iface ptr to interface
  *@return true if header is found
  */
bool poly_parser_seek_header(poly_parser_t* pParser, poly_interface_t* iface);

/**
  *@brief parses packet from data buffer
  *@param pParser ptr to poly parser
  *@param interface index of interface
  *@return PACKET_VALID if packet is ok
  *@return PACKET_INCOMPLETE if len is shorter than packet header indicates
  *@return PACKET_BAD_CHECKSUM if the checksum is incorrect (likely bit error)
  *@return PACKET_PARSING_ERROR if len is longer than it should be (likely missed a delimiter)
  */
ePacketStatus poly_parser_try_parse(poly_parser_t* pParser, int interface);







#ifdef __cplusplus
}
#endif
