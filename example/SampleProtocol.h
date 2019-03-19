/**
  *@file SampleProtocol.h
  *@brief generated protocol source code
  *@author make_protocol.py
  *@date 03/19/19
  */

/***********************************************************
        THIS FILE IS AUTOGENERATED. DO NOT MODIFY
***********************************************************/
#include "Utilities/PolyPacket/poly_field.h"
#include "Utilities/PolyPacket/poly_packet.h"


//Declare extern packet descriptors
extern poly_packet_desc_t* PP_ack;
extern poly_packet_desc_t* PP_SetData;
extern poly_packet_desc_t* PP_GetData;
extern poly_packet_desc_t* PP_RespData;
extern poly_packet_desc_t* PP_blockReq;
extern poly_packet_desc_t* PP_blockResp;


//Declare extern field descriptors
extern poly_field_desc_t* PF_src;
extern poly_field_desc_t* PF_dst;
extern poly_field_desc_t* PF_cmd;
extern poly_field_desc_t* PF_sensorA;
extern poly_field_desc_t* PF_sensorB;
extern poly_field_desc_t* PF_sensorName;
extern poly_field_desc_t* PF_blockOffset;
extern poly_field_desc_t* PF_blockSize;
extern poly_field_desc_t* PF_blockData;


//Structs for packet types//Struct for SetData Packet
//Message to set data in node Packet
typedef struct{
  uint16 msrc;	//Source address of message
  uint16 mdst;	//Desitination address of message
  int16 msensorA;	//Value of Sensor A
  int msensorB;	//Value of Sensor B
  string msensorName[32];	//Name of sensor
} setdata_packet_t;

//Struct for GetData Packet
//Message to get data from node Packet
typedef struct{
  uint16 msrc;	//Source address of message
  uint16 mdst;	//Desitination address of message
  int16 msensorA;	//Value of Sensor A
  int msensorB;	//Value of Sensor B
  string msensorName[32];	//Name of sensor
} getdata_packet_t;

//Struct for RespData Packet
//Response to get/set messages Packet
typedef struct{
  uint16 msrc;	//Source address of message
  uint16 mdst;	//Desitination address of message
  int16 msensorA;	//Value of Sensor A
  int msensorB;	//Value of Sensor B
  string msensorName[32];	//Name of sensor
} respdata_packet_t;

//Struct for blockReq Packet
//This packet is used to request a block of data from the host during Ota updates Packet
typedef struct{
  uint16 msrc;	//Source address of message
  uint16 mdst;	//Desitination address of message
  uint32 mblockOffset;	//Offset of block being requested
  uint32 mblockSize;	//Size of block being requested 
} blockreq_packet_t;

//Struct for blockResp Packet
//This packet sends a block of ota data to the node as a response to a block request Packet
typedef struct{
  uint16 msrc;	//Source address of message
  uint16 mdst;	//Desitination address of message
  uint32 mblockOffset;	//Offset of block in memory
  uint32 mblockSize;	//size of memory block
  uint8 mblockData[64];	//actual data from memory
} blockresp_packet_t;

ack_packet_t* new_ack();
setdata_packet_t* new_setdata();
getdata_packet_t* new_getdata();
respdata_packet_t* new_respdata();
blockreq_packet_t* new_blockreq();
blockresp_packet_t* new_blockresp();


/**
  *@brief initializes protocol service
  *@param ifaces number of interfaces to use
  */

void sp_protocol_init(int ifaces);

/**
  *@brief processes data in buffers
  */

void sp_protocol_process();

/*@brief weak Handler for ack packets */
__weak uint8_t sp_ack_handler(ack_packet_t * packet, int interface);

/*@brief weak Handler for SetData packets */
__weak uint8_t sp_setdata_handler(setdata_packet_t * packet, int interface);

/*@brief weak Handler for GetData packets */
__weak uint8_t sp_getdata_handler(getdata_packet_t * packet, int interface);

/*@brief weak Handler for RespData packets */
__weak uint8_t sp_respdata_handler(respdata_packet_t * packet, int interface);

/*@brief weak Handler for blockReq packets */
__weak uint8_t sp_blockreq_handler(blockreq_packet_t * packet, int interface);

/*@brief weak Handler for blockResp packets */
__weak uint8_t sp_blockresp_handler(blockresp_packet_t * packet, int interface);

/*@brief weak Handler for blockResp packets */
