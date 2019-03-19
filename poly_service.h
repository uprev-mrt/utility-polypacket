/**
  *@file poly_service.h
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

#include "Utilities/Fifo/Fifo.h"
#include "poly_packet.h"



#ifdef __cplusplus

extern "C"
{
#endif

typedef void (*poly_rx_callback)(poly_packet_t* packet, int iface);

typedef enum parseState {
  STATE_WAITING_FOR_HEADER,
  STATE_HEADER_FOUND,
}eParseState;

typedef struct{
  uint8_t* mRaw; //raw packet being parsed
  int mIdx;      //index of raw message

  fifo_t mBytefifo;       //fifo of incoming bytes
  poly_packet_hdr_t mCurrentHdr; //header for current message candidate
  eParseState mParseState;
  fifo_t mPacketBuffer; //outgoing packet buffer
  bool mUpdate;         //flag set when there is new data to process
  //diagnostic info
  int mPacketsIn;     //Total number of incoming packets parsed
  int mPacketsOut;    //Total packets sent on on spool
  int mRetries;       //total number of packet retries (no ack)
  int mFailures;      //total number of packets that failed (hit max retries )
  int mBitErrors;     //total number of bit errors (bad packet parse)
  poly_rx_callback mCallback;
}poly_interface_t;

/**
  *@brief packet handling service
  */
typedef struct{
  int mInterfaceCount;
  poly_interface_t* mInterfaces;
  poly_packet_desc_t** mPacketDescs;
  int mDescCount;
  int mMaxDescs;
  int mMaxPacketSize;
  bool mStarted;
}poly_service_t;


/**
  *@brief create new service
  *@param maxDesc max number of Packet descriptors in protocol
  *@param interfaceCount number of interfaces to allocate
  *@return ptr to newly allocated service
  */
poly_service_t* new_poly_service(int maxDescs, int interfaceCount);

/**
  *@brief register packet descriptor with service
  *@param pService ptr to poly_service
  *@param pDesc packet descriptor to be registered
  *@post Once all packets descriptors are registered the service can be started
  */
void poly_service_register_desc(poly_service_t* pService, poly_packet_desc_t* pDesc);

/**
  *@brief registers a packet received callback for a given interface
  *@param pService ptr to poly service
  *@param interface index of interface to register callback with
  *@post callback callback function
  */
void poly_service_register_rx_callback(poly_service_t* pService, int interface, poly_rx_callback* callback);

/**
  *@brief Starts the service with a given number of interfaces
  *@param pService ptr to service
  *@param fifoDepth how many objects the parsed packet fifo should hold
  *@pre must register all descriptors first
  */
void poly_service_start(poly_service_t* pService, int fifoDepth);


/**
  *@brief 'Feeds' the service bytes to parse
  *@param pService ptr to poly service
  *@param interface index of interface to feed
  *@param data ptr to data being added
  *@param len number of bytes being fed to service
  */
void poly_service_feed(poly_service_t* pService, int interface, uint8_t* data, int len);

/**
  *@brief advances the idx in the interface buffer until the next valid header
  *@param pService ptr to poly service
  *@param iface ptr to interface
  *@return true if header is found
  */
bool poly_service_seek_header(poly_service_t* pService, poly_interface_t* iface);

/**
  *@brief parses packet from interface
  *@param pService ptr to poly service
  *@param packet ptr to store packet if found
  *@param interface index of interface
  *@return PACKET_VALID if packet is ok
  *@return PACKET_INCOMPLETE if len is shorter than packet header indicates
  *@return PACKET_BAD_CHECKSUM if the checksum is incorrect (likely bit error)
  *@return PACKET_PARSING_ERROR if len is longer than it should be (likely missed a delimiter)
  */
ePacketStatus poly_service_try_parse_interface(poly_service_t* pService, poly_packet_t* packet,  int interface);

/**
  *@brief parses packet from data buffer
  *@param pService ptr to poly service
  *@param packet ptr to store packet if found
  *@return PACKET_VALID if packet is ok
  *@return PACKET_NONE is no valid packets are found
  */
ePacketStatus poly_service_try_parse(poly_service_t* pService, poly_packet_t* packet);







#ifdef __cplusplus
}
#endif