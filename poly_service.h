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

#include "Utilities/COBS/cobs_fifo.h"
#include "poly_packet.h"
#include "poly_spool.h"



#ifdef __cplusplus

extern "C"
{
#endif


typedef enum ServiceParseState {
  STATE_WAITING_FOR_HEADER,
  STATE_HEADER_FOUND,
}ServiceParseState_e;

typedef enum HandlerStatus {
  PACKET_SPOOLED,
  PACKET_SENT,
  PACKET_HANDLED,
  PACKET_ROUTED,
  PACKET_IGNORED,
  PACKET_NOT_HANDLED
} HandlerStatus_e;

typedef HandlerStatus_e (*poly_tx_bytes_callback)(uint8_t* data , int len);
typedef HandlerStatus_e (*poly_tx_packet_callback)(poly_packet_t* packet );



typedef struct {
  /*    Incoming      */
  cobs_fifo_t mBytefifo;                 //fifo of incoming bytes
  poly_packet_hdr_t mCurrentHdr;    //header for current message candidate
  /*    Outgoing      */
  poly_spool_t mOutSpool;           //ack/rety spool for outgoing messages
  poly_tx_bytes_callback f_TxBytes;    //call back for writing bytes to interface
  poly_tx_packet_callback f_TxPacket; //call back for writing entire packet
  /*    Diagnostic      */
  int mPacketsIn;     //Total number of incoming packets parsed
  int mPacketsOut;    //Total packets sent on on spool
  bool mTxReady;
}poly_interface_t;

/**
  *@brief packet handling service
  */
typedef struct {
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
  *@brief deinitialize service
  *@param service ptr to service
  */
void poly_service_deinit(poly_service_t* service);

/**
  *@brief register packet descriptor with service
  *@param pService ptr to poly_service
  *@param pDesc packet descriptor to be registered
  *@post Once all packets descriptors are registered the service can be started
  */
void poly_service_register_desc(poly_service_t* pService, poly_packet_desc_t* pDesc);

/**
  *@brief registers a bytes tx callback for an interface, used to send messages as encoded bytes
  *@param pService ptr to poly service
  *@param interface index of interface to register callback with
  *@post callback callback function
  */
void poly_service_register_bytes_tx_callback(poly_service_t* pService, int interface, poly_tx_bytes_callback callback);

/**
  *@brief registers a packet tx callback for an interface, used to send packets
  *@param pService ptr to poly service
  *@param interface index of interface to register callback with
  *@post callback callback function
  */
void poly_service_register_packet_tx_callback(poly_service_t* pService, int interface, poly_tx_packet_callback callback);

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
void poly_service_feed(poly_service_t* pService, int interface,const uint8_t* data, int len);


/**
  *@brief 'Feeds' the service a complete json message
  *@param pService ptr to poly service
  *@param interface index of interface to feed
  *@param data ptr to chars being added
  *@param len number of chars being fed to service
  */
void poly_service_feed_json_msg(poly_service_t* pService, int interface,const char* msg, int len);

/**
  *@brief sets retry behavior for interface of service
  *@param iface index of interface to send on
  *@param retries number of retries
  *@param timeoutMs MS value for timeout before retry
  */
void poly_service_set_retry(poly_service_t* pService, int iface, uint16_t retries, uint32_t timeoutMs);


/**
  *@brief parses packet from json
  *@param pService ptr to poly service
  *@param packet ptr to store packet if found
  *@param msg json string
  *@param len length of json string
  *@return PACKET_VALID if packet is ok
  *@return PACKET_PARSING_ERROR if len is longer than it should be (likely missed a delimiter)
  */
ParseStatus_e poly_service_parse_json(poly_service_t* pService, poly_packet_t* packet ,const char* msg, int len);


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
ParseStatus_e poly_service_try_parse_interface(poly_service_t* pService, poly_packet_t* packet,  poly_interface_t* interface);

/**
  *@brief parses packet from data buffer
  *@param pService ptr to poly service
  *@param packet ptr to store packet if found
  *@return PACKET_VALID if packet is ok
  *@return PACKET_NONE is no valid packets are found
  */
ParseStatus_e poly_service_try_parse(poly_service_t* pService, poly_packet_t* packet);

/**
  *@brief pushes a packet to the spool
  *@param pService ptr to service
  *@param int interface idx of interface to add packet to
  *@param packet packet to add to spool
  *@return "Return of the function"
  */
HandlerStatus_e poly_service_spool(poly_service_t* pService, int interface,  poly_packet_t* packet);

/**
  *@brief grabs next available packet from interface
  *@param interface ptr to interface
  *@param data ptr to store data
  */
bool poly_service_despool_interface(poly_interface_t* iface, poly_packet_t* packet);

/**
  *@brief grabs next available packet from spool and sends it
  *@param pService ptr to service
  */
bool poly_service_despool(poly_service_t* pService);

/**
  *@brief tells the service time has passed so it can track packets timeouts on the spool
  *@param pService ptr to service
  *@param ms number of milliseconds passed
  */
void poly_service_tick(poly_service_t* pService, uint32_t ms);




#ifdef __cplusplus
}
#endif
