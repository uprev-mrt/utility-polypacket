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

#include "Utilities/Fifo/fifo.h"
#include "poly_packet.h"



#ifdef __cplusplus

extern "C"
{
#endif

typedef enum ServiceParseState {
  STATE_WAITING_FOR_HEADER,
  STATE_HEADER_FOUND,
}ServiceParseState_e;

typedef enum HandlerStatus {
  PACKET_SENT,
  PACKET_HANDLED,
  PACKET_ROUTED,
  PACKET_IGNORED,
  PACKET_UNHANDLED
} HandlerStatus_e;

typedef HandlerStatus_e (*poly_tx_callback)(uint8_t* data , int len);


typedef struct{
  uint8_t* mRaw; //raw packet being parsed
  int mIdx;      //index of raw message

  fifo_t mBytefifo;       //fifo of incoming bytes
  poly_packet_hdr_t mCurrentHdr; //header for current message candidate
  ServiceParseState_e mParseState;
  fifo_t mPacketBuffer; //outgoing packet buffer
  bool mUpdate;         //flag set when there is new data to process
  poly_tx_callback f_TxCallBack;
  bool mHasCallBack;
  //diagnostic info
  int mPacketsIn;     //Total number of incoming packets parsed
  int mPacketsOut;    //Total packets sent on on spool
  int mRetries;       //total number of packet retries (no ack)
  int mFailures;      //total number of packets that failed (hit max retries )
  int mBitErrors;     //total number of bit errors (bad packet parse)
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
  bool mAutoAck;
  bool mStarted;
}poly_service_t;


/**
  *@brief create new service
  *@param maxDesc max number of Packet descriptors in protocol
  *@param interfaceCount number of interfaces to allocate
  *@return ptr to newly allocated service
  */
void poly_service_init(poly_service_t* service, int maxDescs, int interfaceCount);

/**
  *@brief register packet descriptor with service
  *@param pService ptr to poly_service
  *@param pDesc packet descriptor to be registered
  *@post Once all packets descriptors are registered the service can be started
  */
void poly_service_register_desc(poly_service_t* pService, poly_packet_desc_t* pDesc);

/**
  *@brief registers a tx callback for an interface, used to send messages
  *@param pService ptr to poly service
  *@param interface index of interface to register callback with
  *@post callback callback function
  */
void poly_service_register_tx_callback(poly_service_t* pService, int interface, poly_tx_callback callback);

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
ParseStatus_e poly_service_try_parse_interface(poly_service_t* pService, poly_packet_t* packet,  int interface);

/**
  *@brief parses packet from data buffer
  *@param pService ptr to poly service
  *@param packet ptr to store packet if found
  *@return PACKET_VALID if packet is ok
  *@return PACKET_NONE is no valid packets are found
  */
ParseStatus_e poly_service_try_parse(poly_service_t* pService, poly_packet_t* packet);

/**
  *@brief sends a packet over a give interface
  *@param "Param description"
  *@pre "Pre-conditions"
  *@post "Post-conditions"
  *@return "Return of the function"
  */
ParseStatus_e poly_service_send(poly_service_t* pService, int interface,  poly_packet_t* packet);







#ifdef __cplusplus
}
#endif
