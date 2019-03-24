/**
  *@file MinimalService.h
  *@brief generated code for Minimal packet service
  *@author make_protocol.py
  *@date 03/24/19
  *@hash A0CADB6E
  */

/***********************************************************
        THIS FILE IS AUTOGENERATED. DO NOT MODIFY
***********************************************************/
#include "Utilities/PolyPacket/poly_service.h"

#define PP_SERVICE_HASH 0xA0CADB6E

/*******************************************************************************
  Enums
*******************************************************************************/

/*******************************************************************************
  Bits/Flags
*******************************************************************************/

/*******************************************************************************
  Global Descriptors
*******************************************************************************/
//Declare extern packet descriptors
extern poly_packet_desc_t* PP_PACKET_ACK;
extern poly_packet_desc_t* PP_PACKET_AMBIENTDATA;
extern poly_packet_desc_t* PP_PACKET_REQUESTAMBIENT;


//Declare extern field descriptors
extern poly_field_desc_t* PP_FIELD_AMBIENT;

/*@brief The main type dealt with by the user
 *@note just wraps a poly_packet to prevent mixing them when multiple protocol are in use
 */
typedef struct{
  poly_packet_t mPacket;    //internal packet structure
  bool mSpooled;            //spooled data doesnt get cleaned, the spool owns it now
  bool mBuilt;
}pp_packet_t;


/*******************************************************************************
  Service Functions
*******************************************************************************/
/**
  *@brief initializes protocol service
  *@param ifaces number of interfaces to use
  */
void pp_service_init(int interfaceCount);

/**
  *@brief tears down service
  *@note probably not needed based on lifecycle of service
  *@ but useful for detecting memory leaks 
  */
void pp_service_teardown();

/**
  *@brief processes data in buffers
  */
void pp_service_process();

/**
  *@brief registers a callback to let the service know how to send bytes for a given interface
  *@param iface index of interface to register with
  *@param txCallBack a function pointer for the callback
  */
void pp_service_register_tx( int iface, poly_tx_callback txCallBack);

/**
  *@brief 'Feeds' bytes to service at given interface for processing
  *@param iface index of interface to send on
  *@param data data to be processed
  *@param number of bytes
  */
void pp_service_feed(int iface, uint8_t* data, int len);

/**
  *@brief sends packet over given interface
  *@param packet packet to be sent
  *@param iface index of interface to send on
  */
HandlerStatus_e pp_send( int iface, pp_packet_t* packet);

/**
  *@brief enables/disables the auto acknowledgement function of the service
  *@param enable true enable auto acks, false disables them
  */
void pp_auto_ack(bool enable);


/*******************************************************************************
  Meta-Packet Functions
*******************************************************************************/

/**
  *@brief initializes a new {proto.prefix}_packet_t
  *@param desc ptr to packet descriptor to model packet from
  */
void pp_packet_build(pp_packet_t* packet, poly_packet_desc_t* desc);


/**
  *@brief recrusively cleans packet and its contents if it still has ownership
  *@param packet packet to clean
  */
void pp_clean(pp_packet_t* packet);

/**
  *@brief converts packet to json
  *@param packet ptr to packet to convert
  *@param buf buffer to store string
  *@return length of string
  */
#define pp_print_json(packet,buf) poly_packet_print_json(&(packet)->mPacket, buf, false)

/**
  *@brief parses packet from a buffer of data
  *@param packet ptr to packet to be built
  *@param buf buffer to parse
  *@return status of parse attempt
  */
#define pp_parse(packet,buf,len) poly_packet_parse_buffer(&(packet)->mPacket, buf, len)


/**
  *@brief packs packet into a byte array
  *@param packet ptr to packet to be packed
  *@param buf buffer to store data
  *@return length of packed data
  */
#define pp_pack(packet, buf) poly_packet_pack(&(packet)->mPacket, buf)

/**
  *@brief gets the length of a give field in a packet
  *@param packet ptr to pp_packet_t
  *@param field ptr to field descriptor
  *@return size of field
  */
int pp_fieldLen(pp_packet_t* packet, poly_field_desc_t* fieldDesc );

/*******************************************************************************
  Meta-Packet setters
*******************************************************************************/
void pp_setAmbient(pp_packet_t* packet, int val);

/*******************************************************************************
  Meta-Packet getters
*******************************************************************************/
int pp_getAmbient(pp_packet_t* packet);

/*******************************************************************************
  Quick send functions

  These are convenience one-liner functions for sending messages.
  They also handle their own clean up and are less bug prone than building your own packets
*******************************************************************************/
HandlerStatus_e pp_sendAck(int iface);
HandlerStatus_e pp_sendAmbientData(int iface, int ambient);
HandlerStatus_e pp_sendRequestAmbient(int iface);

/*******************************************************************************
  Packet Handlers
*******************************************************************************/
/*@brief Handler for ack packets */
HandlerStatus_e pp_ack_handler(pp_packet_t* pp_ack);

/*@brief Handler for AmbientData packets */
HandlerStatus_e pp_AmbientData_handler(pp_packet_t* pp_AmbientData);

/*@brief Handler for RequestAmbient packets */
HandlerStatus_e pp_RequestAmbient_handler(pp_packet_t* pp_RequestAmbient, pp_packet_t* pp_AmbientData);

/*@brief Catch-All Handler for unhandled packets */
HandlerStatus_e pp_default_handler(pp_packet_t * pp_packet);