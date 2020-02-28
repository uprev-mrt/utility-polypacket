/**
  *@file testService.h
  *@brief generated code for test packet service
  *@author make_protocol.py
  *@date 02/28/20
  *@hash BE5C1C7E
  */

#pragma once
/***********************************************************
        THIS FILE IS AUTOGENERATED. DO NOT MODIFY
***********************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "Utilities/PolyPacket/poly_service.h"

#ifdef __cplusplus
}
#endif

#define TP_SERVICE_HASH 0xBE5C1C7E

/*******************************************************************************
  Enums
*******************************************************************************/
/* Enums for cmd field */
typedef enum{
  TP_CMD_LED_ON,              /* turns on led */
  TP_CMD_LED_OFF,              /* turns off led */
  TP_CMD_RESET,              /* resets device */
  TP_CMD_MAX_LIMIT
} tp_cmd_e;


/*******************************************************************************
  Bits/Flags
*******************************************************************************/

/*******************************************************************************
  Global Descriptors
*******************************************************************************/
//Declare extern packet descriptors
extern poly_packet_desc_t* TP_PACKET_PING;
extern poly_packet_desc_t* TP_PACKET_ACK;
extern poly_packet_desc_t* TP_PACKET_SENDCMD;
extern poly_packet_desc_t* TP_PACKET_GETDATA;
extern poly_packet_desc_t* TP_PACKET_DATA;

extern poly_packet_desc_t* TP_STRUCT_NODE;


//Declare extern field descriptors
extern poly_field_desc_t* TP_FIELD_ICD;
extern poly_field_desc_t* TP_FIELD_SENSORA;
extern poly_field_desc_t* TP_FIELD_SENSORB;
extern poly_field_desc_t* TP_FIELD_SENSORNAME;
extern poly_field_desc_t* TP_FIELD_CMD;

/*
 *@brief The main type dealt with by the user
 */

typedef poly_packet_t tp_packet_t;
typedef tp_packet_t tp_struct_t;


/*******************************************************************************
  Service Functions
*******************************************************************************/
/**
  *@brief initializes protocol service
  *@param ifaces number of interfaces to use
  *@param depth spool size for each interface
  */
void tp_service_init(int interfaceCount, int depth);

/**
  *@brief tears down service
  *@note probably not needed based on lifecycle of service
  *@ but useful for detecting memory leaks
  */
void tp_service_teardown();


/**
  *@brief handles packets and dispatches to handler
  *@param req incoming message
  *@param resp response to message
  *@param number of bytes
  */
HandlerStatus_e tp_service_dispatch(tp_packet_t* req, tp_packet_t* resp);

/**
  *@brief processes data in buffers
  */
void tp_service_process();

/**
  *@brief registers a callback to let the service know how to send bytes for a given interface
  *@param iface index of interface to register with
  *@param txBytesCallBack a function pointer for the callback
  */
void tp_service_register_bytes_tx( int iface, poly_tx_bytes_callback txBytesCallBack);

/**
  *@brief registers a callback to let the service know how to send entire packets
  *@param iface index of interface to register with
  *@param txPacketCallBack a function pointer for the callback
  */
void tp_service_register_packet_tx( int iface, poly_tx_packet_callback txPacketCallBack);

/**
  *@brief 'Feeds' bytes to service at given interface for processing
  *@param iface index of interface to send on
  *@param data data to be processed
  *@param number of bytes
  */
void tp_service_feed(int iface, uint8_t* data, int len);

/**
  *@brief sets retry behavior for interface of service
  *@param iface index of interface to send on
  *@param retries number of retries
  *@param timeoutMs MS value for timeout before retry
  */
void tp_service_set_retry(int iface, uint16_t retries, uint32_t timeoutMs);

/**
  *@brief handles json message, and shortcuts the servicing proccess. used for http requests
  *@param req incoming json message string
  *@param resp response data
  *@param number of bytes
  */
HandlerStatus_e tp_handle_json(const char* req,int len, char* resp);

/**
  *@brief 'Feeds' json message to service
  *@param iface index of interface to send on
  *@param msg data to be processed
  *@param number of bytes
  */
void tp_service_feed_json(int iface, const char* msg, int len);

/**
  *@brief sends packet over given interface
  *@param packet packet to be sent
  *@param iface index of interface to send on
  */
HandlerStatus_e tp_send( int iface, tp_packet_t* packet);

/**
  *@brief tells the service time has passed so it can track packets timeouts/retries on the spool
  *@param ms number of milliseconds passed
  *@note this only sets flags/statuses. Nothing is handled until the next call to process the service. So it is fine to call this from a systick handler
  */
void tp_tick(uint32_t ms);

/**
  *@brief enables/disables the auto acknowledgement function of the service
  *@param enable true enable auto acks, false disables them
  */
void tp_auto_ack(bool enable);

/**
  *@brief enables/disables the txReady of an interface
  *@param enable true enable auto acks, false disables them
  */
void tp_enable_tx(int iface);
void tp_disable_tx(int iface);


/*******************************************************************************
  Meta-Packet Functions
*******************************************************************************/

/**
  *@brief initializes a new {proto.prefix}_packet_t
  *@param desc ptr to packet descriptor to model packet from
  */
void tp_packet_build(tp_packet_t* packet, poly_packet_desc_t* desc);
#define tp_struct_build(packet,desc) tp_packet_build(packet,desc)



/**
  *@brief recrusively cleans packet and its contents if it still has ownership
  *@param packet packet to clean
  */
void tp_clean(tp_packet_t* packet);

/**
  *@brief converts packet to json
  *@param packet ptr to packet to convert
  *@param buf buffer to store string
  *@return length of string
  */
#define tp_print_json(packet,buf) poly_packet_print_json((packet), buf, false)

/**
  *@brief parses packet from a buffer of data
  *@param packet ptr to packet to be built
  *@param buf buffer to parse
  *@return status of parse attempt
  */
#define tp_parse(packet,buf,len) poly_packet_parse_buffer((packet), buf, len)

/**
  *@brief Copies all fields present in both packets from src to dst
  *@param dst ptr to packet to copy to
  *@param src ptr to packet to copy from
  */
#define tp_packet_copy(dst,src) poly_packet_copy((dst),(src) )

/**
  *@brief packs packet into a byte array
  *@param packet ptr to packet to be packed
  *@param buf buffer to store data
  *@return length of packed data
  */
#define tp_pack(packet, buf) poly_packet_pack((packet), buf)

/**
  *@brief gets the descriptor for the packet
  *@param packet ptr to packet to be checked
  */
#define tp_getDesc(packet) ((packet).mDesc)


/*******************************************************************************
  Meta-Packet setters
*******************************************************************************/
void tp_setIcd(tp_packet_t* packet, uint32_t val);
void tp_setSensorA(tp_packet_t* packet, int16_t val);
void tp_setSensorB(tp_packet_t* packet, int val);
void tp_setSensorName(tp_packet_t* packet, const char* val);
void tp_setCmd(tp_packet_t* packet, uint8_t val);

/*******************************************************************************
  Meta-Packet getters
*******************************************************************************/

/**
  *@brief checks to see if field is present in packet
  *@param packet ptr to packet to be packed
  *@param field ptr to field desc
  *@return true if field is present
  */
#define tp_hasField(packet, field) poly_packet_has((packet), field)

uint32_t tp_getIcd(tp_packet_t* packet);
int16_t tp_getSensorA(tp_packet_t* packet);
int tp_getSensorB(tp_packet_t* packet);
void tp_getSensorName(tp_packet_t* packet, char* val);
uint8_t tp_getCmd(tp_packet_t* packet);

/*******************************************************************************
  Quick send functions

  These are convenience one-liner functions for sending messages.
  They also handle their own clean up and are less bug prone than building your own packets
*******************************************************************************/

/**
  *@brief Sends a ping
  *@param iface interface to ping
  *@note a ping is just an ACK without the ack flag set in the token
  */
HandlerStatus_e tp_sendPing(int iface);

HandlerStatus_e tp_sendSendCmd(int iface);
HandlerStatus_e tp_sendGetData(int iface);
HandlerStatus_e tp_sendData(int iface);

/*******************************************************************************
  Packet Handlers
*******************************************************************************/
/*@brief Handler for Ping packets */
HandlerStatus_e tp_Ping_handler(tp_packet_t* tp_Ping, tp_packet_t* tp_Ack);

/*@brief Handler for Ack packets */
HandlerStatus_e tp_Ack_handler(tp_packet_t* tp_Ack);

/*@brief Handler for SendCmd packets */
HandlerStatus_e tp_SendCmd_handler(tp_packet_t* tp_SendCmd);

/*@brief Handler for GetData packets */
HandlerStatus_e tp_GetData_handler(tp_packet_t* tp_GetData, tp_packet_t* tp_Data);

/*@brief Handler for Data packets */
HandlerStatus_e tp_Data_handler(tp_packet_t* tp_Data);

/*@brief Catch-All Handler for unhandled packets */
HandlerStatus_e tp_default_handler(tp_packet_t * tp_packet, tp_packet_t * tp_response);