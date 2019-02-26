/**
  *@file SampleProtocol.h
  *@brief generated protocol source code
  *@author make_protocol.py
  *@date 02/24/19
  */

/***********************************************************
        THIS FILE IS AUTOGENERATED. DO NOT MODIFY
***********************************************************/
#include "var_field.h"
#include "var_packet.h"


//Declare extern packet descriptors
extern packet_desc_t* VP_SETDATA;
extern packet_desc_t* VP_GETDATA;
extern packet_desc_t* VP_RESPDATA;
extern packet_desc_t* VP_BLOCKREQ;
extern packet_desc_t* VP_BLOCKRESP;


//Declare extern field descriptors
extern field_desc_t* VF_SRC;
extern field_desc_t* VF_DST;
extern field_desc_t* VF_CMD;
extern field_desc_t* VF_SENSORA;
extern field_desc_t* VF_SENSORB;
extern field_desc_t* VF_SENSORNAME;
extern field_desc_t* VF_BLOCKOFFSET;
extern field_desc_t* VF_BLOCKSIZE;
extern field_desc_t* VF_BLOCKDATA;


//Structs for packet types//Struct for SetData Packet
//Message to set data in node Packet
typedef struct{
  uint16_t msrc;	//Source address of message
  uint16_t mdst;	//Desitination address of message
  int16 msensorA;	//Value of Sensor A
  int msensorB;	//Value of Sensor B
  char msensorName[16];	//Name of sensor
} setdata_struct_t;

setdata_struct_t* new_setdata();//Struct for GetData Packet
//Message to get data from node Packet
typedef struct{
  uint16_t msrc;	//Source address of message
  uint16_t mdst;	//Desitination address of message
  int16 msensorA;	//Value of Sensor A
  int msensorB;	//Value of Sensor B
  char msensorName[16];	//Name of sensor
} getdata_struct_t;

getdata_struct_t* new_getdata();//Struct for RespData Packet
//Response to get/set messages Packet
typedef struct{
  uint16_t msrc;	//Source address of message
  uint16_t mdst;	//Desitination address of message
  int16 msensorA;	//Value of Sensor A
  int msensorB;	//Value of Sensor B
  char msensorName[16];	//Name of sensor
} respdata_struct_t;

respdata_struct_t* new_respdata();//Struct for blockReq Packet
//This packet is used to request a block of data from the host during Ota updates Packet
typedef struct{
  uint16_t msrc;	//Source address of message
  uint16_t mdst;	//Desitination address of message
  uint32_t mblockOffset;	//Offset of block being requested
  uint32_t mblockSize;	//Size of block being requested 
} blockreq_struct_t;

blockreq_struct_t* new_blockreq();//Struct for blockResp Packet
//This packet sends a block of ota data to the node as a response to a block request Packet
typedef struct{
  uint16_t msrc;	//Source address of message
  uint16_t mdst;	//Desitination address of message
  uint32_t mblockOffset;	//Offset of block in memory
  uint32_t mblockSize;	//size of memory block
  uint8_t mblockData[64];	//actual data from memory
} blockresp_struct_t;

blockresp_struct_t* new_blockresp();

void SampleProtocolprotocol_init();