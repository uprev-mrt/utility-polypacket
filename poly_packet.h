/**
  *@file poly_packet.h
  *@brief header for variable packet module
  *@author Jason Berger
  *@date 02/19/2019
  */

#include "poly_field.h"


enum ePacketStatus {
  PACKET_VALID = -400,
  PACKET_INCOMPLETE,
  PACKET_BAD_CHECKSUM,
  PACKET_PARSING_ERROR,
  INVALID_PACKET_TYPE
};

//[ 1 byte payloadId] [ 2 byte payload len ]  [2 byte token] [ mManifestSize bytes manifest] [n byte variable data ] [2 byte checksum]

/**
  *@brief Variable Packet Descriptor
  */
typedef struct{
  uint8_t mTypeId;       //unique id for packet type
  const char* mName;        //friendly name for packet type
  poly_field_desc_t** mFields;   //Array of field descriptors
  int mMaxFields;           //max fields
  int mFieldCount;          //number of field descriptors
  uin8_t mManifestSize;     //size in bytes of manifest
}poly_packet_desc_t;


typedef struct{
  uint8_t mTypeId; //id of payload type
  uint16_t mDataLen;  //expected len of packet data (not including header and footer)
  uint16_t mToken;    //token for packet (used for acknowledgement/ echo cancellation in mesh nets)
}poly_packet_hdr_t;

typedef struct{
  uint16_t mCheckSum; //checksum of packet data
}poly_packet_ftr_t;

/**
  *@brief Variable packet
  */
typedef struct{
  poly_packet_hdr_t mHdr;
  poly_packet_desc_t* mDesc;     //prt to packet descriptor
  poly_field_t* mFields;     //array of fields contained in packet
  poly_packet_ftr_t mFooter; //packet footer
}poly_packet_t;


/**
  *@brief creates a new packet descriptor
  *@param name firendly name for packet type
  *@param maxFields max number of fields in packet descriptor
  *@return ptr to new packet descriptor
  */
poly_packet_desc_t* new_poly_packet_desc(const char* name , int maxFields);

/**
  *@brief adds field descriptor to packet descriptor
  *@param poly_packet_desc ptr to packet descriptor
  *@param poly_field_desc ptr to field descriptor to be added
  */
void poly_packet_desc_add_field(poly_packet_desc_t* desc, poly_field_desc_t* fieldDesc);


/**
  *@brief gets the ID from a raw data buffer
  *@param data raw data to be identified and parsed
  *@param len length of data buffer
  *@return type id of packet
  *@return PACKET_INCOMPLETE if len is to short to identify
  */
int poly_packet_id(uin8_t* data, int len);

/**
  *@brief parses a packet from raw data buffer
  *@param packet ptr to packet being parsed
  *@param packet descriptor to parsing data
  *@param data raw data to be parsed
  *@param len length of data available for parsing
  *@pre Must be bound to data struct before this is called
  *@return PACKET_VALID if packet is ok
  *@return PACKET_INCOMPLETE if len is shorter than packet header indicates
  *@return PACKET_BAD_CHECKSUM if the checksum is incorrect (likely bit error)
  *@return PACKET_PARSING_ERROR if len is longer than it should be (likely missed a delimiter)
  */
ePacketStatus poly_packet_parse(poly_packet_t* packet, poly_packet_desc_t* dec, uin8_t* data, int len);
