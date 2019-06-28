/**
  *@file poly_packet.h
  *@brief header for variable packet module
  *@author Jason Berger
  *@date 02/19/2019
  */

#pragma once
#include "poly_field.h"

#ifdef __cplusplus

#if defined(POLY_PACKET_DEBUG_LVL)
extern char POLY_DEBUG_PRINTBUF[512];
#endif

extern "C"
{
#endif

//Ensure we pack structs
#pragma pack(push)
#pragma pack(1)

//forward declare struct so we can typedef callbacks
struct poly_packet;

typedef void (*packet_ack_cb)(struct poly_packet* response);
typedef void (*packet_failed_cb)();

typedef enum ParseStatus {
  PACKET_VALID = -400,
  PACKET_INCOMPLETE,
  PACKET_BAD_CHECKSUM,
  PACKET_PARSING_ERROR,
  INVALID_PACKET_TYPE,
  PACKET_NONE
} ParseStatus_e;

//Spool entry ack type
typedef enum{
   ACK_TYPE_NONE,         //no ack needed
   ACK_TYPE_TOKEN,        //request ack via token
   ACK_TYPE_PASSTHROUGH  //pass through message, dont touch the token (used when relaying messages so the endpoints can handle ack/retry)
} packet_ack_type_e;


#define PACKET_METADATA_SIZE (sizeof(poly_packet_hdr_t))
#define POLY_ACK_FLAG 0x8000

//[ 1 byte payloadId] [ 2 byte payload len ]  [2 byte token] [ mManifestSize bytes manifest] [n byte variable data ] [2 byte checksum]
/**
  *@brief Variable Packet Descriptor
  */
typedef struct {
  uint8_t mTypeId;              //unique id for packet type
  const char* mName;            //friendly name for packet type
  poly_field_desc_t** mFields;  //Array of field descriptors
  bool* mRequirementMap;        //map of which fields are required
  int mMaxFields;               //max fields
  int mFieldCount;              //number of field descriptors
  uint8_t mOptionalFieldCount;  //number of fields that are optional (used to calc manifest size)
  uint8_t mManifestSize;        //size in bytes of manifest
  int mMaxPacketSize;
}poly_packet_desc_t;


typedef struct {
  uint8_t mTypeId;    //id of payload type
  uint8_t mSeq;       // sequential field to detect dropped packets
  uint16_t mDataLen;  //expected len of packet data (not including header and footer)
  uint16_t mToken;    //token for packet (used for acknowledgement/ echo cancellation in mesh nets)
  uint16_t mCheckSum; //checksum of packet data
}poly_packet_hdr_t;

/**
  *@brief Variable packet
  */
typedef struct poly_packet{
  poly_packet_hdr_t mHeader;
  const poly_packet_desc_t* mDesc;      //prt to packet descriptor
  poly_field_t* mFields;          //array of fields contained in packet
  uint8_t mInterface;              //id of interface that packet is from/to
  bool mBuilt;                    //indicates if packet has already been built
  packet_ack_type_e mAckType;     //indicates what type of ack the packet should use
  packet_ack_cb f_mAckCallback;   //callback for when packet is acknowledged
  packet_failed_cb f_mFailedCallback; //callback for when the packet timesout
  MRT_MUTEX_TYPE mMutex;
}poly_packet_t;

#pragma pack(pop)

/**
  *@brief initializes a packet descriptor
  *@param name firendly name for packet type
  *@param maxFields max number of fields in packet descriptor
  *@return ptr to packet descriptor
  */
poly_packet_desc_t* poly_packet_desc_init(poly_packet_desc_t* desc,int id, const char* name , int maxFields);
poly_packet_desc_t* poly_packet_desc_deinit(poly_packet_desc_t* desc);

/**
  *@brief adds field descriptor to packet descriptor
  *@param poly_packet_desc ptr to packet descriptor
  *@param poly_field_desc ptr to field descriptor to be added
  *@param required indicates if field is a required in packet
  */
void poly_packet_desc_add_field(poly_packet_desc_t* desc, poly_field_desc_t* fieldDesc, bool required);


/**
  *@brief builds a poly_packet from a descriptor
  *@param desc ptr to packet descriptor
  *@param allocate whether or not to allocate the memory
  */
void poly_packet_build(poly_packet_t* packet, const poly_packet_desc_t* desc, bool allocate );

/**
  *@brief cleans up all memory allocated by the packet (but not the packet itself)
  *@param packet ptr to packet being cleaned
  */
void poly_packet_clean(poly_packet_t* packet);


/**
  *@brief copies data from field
  *@param packet ptr to packet
  *@param desc ptr to field descriptor
  *@param data pointer to store value
  *@return 1 on success
  */
int poly_packet_get_field(poly_packet_t* packet, const poly_field_desc_t* desc, void* data);

/**
  *@brief copies data to field
  *@param packet ptr to packet
  *@param desc ptr to field descriptor
  *@param data pointer to store value
  *@return 1 on success
  */
int poly_packet_set_field(poly_packet_t* packet, const poly_field_desc_t* desc, void* data);

/**
  *@brief gets the ID from a raw data buffer
  *@param data raw data to be identified and parsed
  *@param len length of data buffer
  *@return type id of packet
  *@return PACKET_INCOMPLETE if len is to short to identify
  */
int poly_packet_id(uint8_t* data, int len);

/**
  *@brief parses a packet from raw data buffer
  *@param packet ptr to packet being parsed
  *@param data raw data to be parsed
  *@param len length of data available for parsing
  *@pre Must be bound to data struct before this is called
  *@return PACKET_VALID if packet is ok
  *@return PACKET_INCOMPLETE if len is shorter than packet header indicates
  *@return PACKET_BAD_CHECKSUM if the checksum is incorrect (likely bit error)
  *@return PACKET_PARSING_ERROR if len is longer than it should be (likely missed a delimiter)
  */
ParseStatus_e poly_packet_parse_buffer(poly_packet_t* packet, const uint8_t* data, int len);

/**
  *@brief parses a packet from raw data buffer
  *@param packet ptr to packet being parsed
  *@param obj json obj to be parsed
  *@pre Must be bound to data struct before this is called
  *@return PACKET_VALID if packet is ok
  *@return PACKET_INCOMPLETE if len is shorter than packet header indicates
  *@return PACKET_BAD_CHECKSUM if the checksum is incorrect (likely bit error)
  *@return PACKET_PARSING_ERROR if len is longer than it should be (likely missed a delimiter)
  */
ParseStatus_e poly_packet_parse_json_obj(poly_packet_t* packet, json_obj_t* json);


/**
  *@brief packs data into byte array
  *@param packet ptr to packet
  *@pre data ptr to memory to store packed data
  *@return length of packed data
  */
int poly_packet_pack(poly_packet_t* packet, uint8_t* data);

/**
  *@brief packs data into byte array encoded in COB
  *@param packet ptr to packet
  *@pre data ptr to memory to store packed data
  *@return length of packed data
  */
int poly_packet_pack_encoded(poly_packet_t* packet, uint8_t* data);


/**
  *@brief Copies all fields present in both packets from src to dst
  *@param dst ptr to packet to copy to
  *@param src ptr to packet to copy from
  *@return number of fields copied
  */
int poly_packet_copy(poly_packet_t* dst, poly_packet_t* src);

/**
  *@brief prints json representation of packet to a buffer
  *@param packet ptr to packet
  *@param buf buffer to print to
  *@param printHeader indicates if the header data should be printed
  *@retun len of string
  */
int poly_packet_print_json(poly_packet_t* packet, char* buf, bool printHeader);


/**
  *@brief prints hex array of packed packet (mainly useful for debug)
  *@param packet ptr to packet
  *@param buf buffer to print to
  *@retun len of string
  */
int poly_packet_print_packed(poly_packet_t* packet, char* buf);

/**
  *@brief updates the header for the packet
  *@param packet ptr to packet to update
  *@return total length of packet including header
  */
int poly_packet_update_header(poly_packet_t* packet);


#ifdef __cplusplus
}
#endif
