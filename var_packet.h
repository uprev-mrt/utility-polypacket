/**
  *@file var_packet.h
  *@brief header for variable packet module
  *@author Jason Berger
  *@date 02/19/2019
  */

#include "var_field.h"


enum ePacketStatus {
  PACKET_VALID,
  PACKET_INCOMPLETE,
  PACKET_BAD_CHECKSUM,
  PACKET_PARSING_ERROR
};

//[ 1 byte payloadId] [ 2 byte payload len ] [n byte variable data ] [2 byte checksum] [2 byte token]

/**
  *@brief Variable Packet Descriptor
  */
typedef struct{
  uint8_t mPayloadId;       //unique id for packet type
  const char* mName;        //friendly name for packet type
  field_desc_t** mFields;   //Array of field descriptors
  int mMaxFields;           //max fields
  int mFieldCount;          //number of field descriptors
}packet_desc_t;


/**
  *@brief Variable packet
  */
typedef struct{
  uint8_t* mData;         //ptr to raw data of message
  int mLen;               //length of raw message data
  packet_desc_t* mDesc;   //prt to packet descriptor
  var_field_t* mFields;   //array of fields contained in packet
  uint16_t mCheckSum;      //checksum for packet
  int mFieldCount;          //number of field descriptors
  uint32_t mToken;        //token for packet (used for acknowledgement/ echo cancellation in mesh nets)
}var_packet_t;



/**
  *@brief initializes the parsing dictionary, called automatically when the first packet descriptor is created
  */
void parsing_dictionary_init();

/**
  *@brief creates a new packet descriptor
  *@param name firendly name for packet type
  *@return ptr to new packet descriptor
  */
packet_desc_t* new_packet_desc(const char* name);

/**
  *@brief adds field descriptor to packet descriptor
  *@param packet_desc ptr to packet descriptor
  *@param field_desc ptr to field descriptor to be added
  */
void packet_desc_add_field(packet_desc_t* desc, field_desc_t* fieldDesc);


/**
  *@brief creates a new packet from a packet descriptor
  *@param desc ptr to packet descriptor
  *@return ptr to new packet
  */
var_packet_t* new_packet(packet_desc_t* desc);

/**
  *@brief resets a packet to blank
  *@param packet packet to be reset
  */
void packet_reset(var_packet_t* packet);

/**
  *@brief gets handle for field in packet from field descriptor
  *@param fieldDesc ptr to field descriptor
  *@return ptr to field
  */
var_field_t* packet_field(var_packet_t* packet, field_desc_t* fieldDesc);


/**
  *@brief parses packet from data buffer
  *@param packet ptr to packet being filled out
  *@param data ptr to raw data to be parsed
  *@param len length of data buffer to be parsed
  *@return PACKET_VALID if packet is ok
  *@return PACKET_INCOMPLETE if len is shorter than packet header indicates
  *@return PACKET_BAD_CHECKSUM if the checksum is incorrect (likely bit error)
  *@return PACKET_PARSING_ERROR if len is longer than it should be (likely missed a delimiter)
  */
ePacketStatus packet_parse(var_packet_t* packet, uint8_t* data, int len );
