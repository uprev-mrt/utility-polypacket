/**
  *@file SampleProtocol.cpp
  *@brief generated protocol source code
  *@author make_protocol.py
  *@date 03/18/19
  */

/***********************************************************
        THIS FILE IS AUTOGENERATED. DO NOT MODIFY
***********************************************************/

#include "SampleProtocol.h"
#include "Utilities/PolyPacket/poly_parser.h"


//Define packet IDs
#define  ACK_P_DESC_ID 0
#define  SETDATA_P_DESC_ID 1
#define  GETDATA_P_DESC_ID 2
#define  RESPDATA_P_DESC_ID 3
#define  BLOCKREQ_P_DESC_ID 4
#define  BLOCKRESP_P_DESC_ID 5


//Global descriptors
  poly_packet_desc_t* ACK_P_DESC
  poly_packet_desc_t* SETDATA_P_DESC
  poly_packet_desc_t* GETDATA_P_DESC
  poly_packet_desc_t* RESPDATA_P_DESC
  poly_packet_desc_t* BLOCKREQ_P_DESC
  poly_packet_desc_t* BLOCKRESP_P_DESC

  poly_field_desc_t* SRC_F_DESC
  poly_field_desc_t* DST_F_DESC
  poly_field_desc_t* CMD_F_DESC
  poly_field_desc_t* SENSORA_F_DESC
  poly_field_desc_t* SENSORB_F_DESC
  poly_field_desc_t* SENSORNAME_F_DESC
  poly_field_desc_t* BLOCKOFFSET_F_DESC
  poly_field_desc_t* BLOCKSIZE_F_DESC
  poly_field_desc_t* BLOCKDATA_F_DESC

poly_parser_t* sp_PARSER;

/*******************************************************************************
  Service Process
*******************************************************************************/
/**
  *@brief attempts to process data in buffers and parse out packets
  */
void sp_protocol_process()
{
  sp_PARSER
}

/*******************************************************************************
  Service initializer
*******************************************************************************/

/**
  *@brief initializes sp_protocol
  *@param interfaceCount number of interfaces to create
  */
void sp_protocol_init(int interfaceCount)
{

  sp_PARSER = new_poly_parser(6, interfaceCount);

  //Build Packet Descriptors
  ACK_P_DESC = new_poly_packet_desc("ack", 0);
  SETDATA_P_DESC = new_poly_packet_desc("SetData", 5);
  GETDATA_P_DESC = new_poly_packet_desc("GetData", 5);
  RESPDATA_P_DESC = new_poly_packet_desc("RespData", 5);
  BLOCKREQ_P_DESC = new_poly_packet_desc("blockReq", 4);
  BLOCKRESP_P_DESC = new_poly_packet_desc("blockResp", 5);

  //Build Field Descriptors
  SRC_F_DESC = new_poly_field_desc("blockResp", TYPE_UINT16, 1, FORMAT_HEX);
  DST_F_DESC = new_poly_field_desc("blockResp", TYPE_UINT16, 1, FORMAT_HEX);
  CMD_F_DESC = new_poly_field_desc("blockResp", TYPE_UINT8, 1, FORMAT_HEX);
  SENSORA_F_DESC = new_poly_field_desc("blockResp", TYPE_INT16, 1, FORMAT_DEC);
  SENSORB_F_DESC = new_poly_field_desc("blockResp", TYPE_INT, 1, FORMAT_DEC);
  SENSORNAME_F_DESC = new_poly_field_desc("blockResp", TYPE_STRING, 32, FORMAT_ASCII);
  BLOCKOFFSET_F_DESC = new_poly_field_desc("blockResp", TYPE_UINT32, 1, FORMAT_HEX);
  BLOCKSIZE_F_DESC = new_poly_field_desc("blockResp", TYPE_UINT32, 1, FORMAT_DEC);
  BLOCKDATA_F_DESC = new_poly_field_desc("blockResp", TYPE_UINT8, 64, FORMAT_NONE);

  //Settomg Field Descriptors for ack
  //Settomg Field Descriptors for SetData
  poly_packet_desc_add_field(SETDATA_P_DESC , SRC_F_DESC , true );
  poly_packet_desc_add_field(SETDATA_P_DESC , DST_F_DESC , true );
  poly_packet_desc_add_field(SETDATA_P_DESC , SENSORA_F_DESC , false );
  poly_packet_desc_add_field(SETDATA_P_DESC , SENSORB_F_DESC , false );
  poly_packet_desc_add_field(SETDATA_P_DESC , SENSORNAME_F_DESC , false );
  //Settomg Field Descriptors for GetData
  poly_packet_desc_add_field(GETDATA_P_DESC , SRC_F_DESC , true );
  poly_packet_desc_add_field(GETDATA_P_DESC , DST_F_DESC , true );
  poly_packet_desc_add_field(GETDATA_P_DESC , SENSORA_F_DESC , false );
  poly_packet_desc_add_field(GETDATA_P_DESC , SENSORB_F_DESC , false );
  poly_packet_desc_add_field(GETDATA_P_DESC , SENSORNAME_F_DESC , false );
  //Settomg Field Descriptors for RespData
  poly_packet_desc_add_field(RESPDATA_P_DESC , SRC_F_DESC , true );
  poly_packet_desc_add_field(RESPDATA_P_DESC , DST_F_DESC , true );
  poly_packet_desc_add_field(RESPDATA_P_DESC , SENSORA_F_DESC , false );
  poly_packet_desc_add_field(RESPDATA_P_DESC , SENSORB_F_DESC , false );
  poly_packet_desc_add_field(RESPDATA_P_DESC , SENSORNAME_F_DESC , false );
  //Settomg Field Descriptors for blockReq
  poly_packet_desc_add_field(BLOCKREQ_P_DESC , SRC_F_DESC , true );
  poly_packet_desc_add_field(BLOCKREQ_P_DESC , DST_F_DESC , true );
  poly_packet_desc_add_field(BLOCKREQ_P_DESC , BLOCKOFFSET_F_DESC , true );
  poly_packet_desc_add_field(BLOCKREQ_P_DESC , BLOCKSIZE_F_DESC , true );
  //Settomg Field Descriptors for blockResp
  poly_packet_desc_add_field(BLOCKRESP_P_DESC , SRC_F_DESC , true );
  poly_packet_desc_add_field(BLOCKRESP_P_DESC , DST_F_DESC , true );
  poly_packet_desc_add_field(BLOCKRESP_P_DESC , BLOCKOFFSET_F_DESC , true );
  poly_packet_desc_add_field(BLOCKRESP_P_DESC , BLOCKSIZE_F_DESC , true );
  poly_packet_desc_add_field(BLOCKRESP_P_DESC , BLOCKDATA_F_DESC , true );

  //Register packet descriptors with the parser
  poly_parser_register_desc(sp_PARSER, ACK_P_DESC);
  poly_parser_register_desc(sp_PARSER, SETDATA_P_DESC);
  poly_parser_register_desc(sp_PARSER, GETDATA_P_DESC);
  poly_parser_register_desc(sp_PARSER, RESPDATA_P_DESC);
  poly_parser_register_desc(sp_PARSER, BLOCKREQ_P_DESC);
  poly_parser_register_desc(sp_PARSER, BLOCKRESP_P_DESC);

}

/*******************************************************************************
  Meta packet
*******************************************************************************/
void sp_packet_init(sp_packet_t* packet, poly_packet_desc_t* desc)
{
  //set typeId
  packet->mTypeId = desc->mTypeId;

  //create a new unallocated packet
  poly_packet_t* newPacket = new_poly_packet(desc, false);


  switch(packet->mTypeId)
  {
    case ACK_P_DESC_ID:
    sp_ack_bind(packet->mPayload.ack, newPacket);
    break;
    case SETDATA_P_DESC_ID:
    sp_setdata_bind(packet->mPayload.setdata, newPacket);
    break;
    case GETDATA_P_DESC_ID:
    sp_getdata_bind(packet->mPayload.getdata, newPacket);
    break;
    case RESPDATA_P_DESC_ID:
    sp_respdata_bind(packet->mPayload.respdata, newPacket);
    break;
    case BLOCKREQ_P_DESC_ID:
    sp_blockreq_bind(packet->mPayload.blockreq, newPacket);
    break;
    case BLOCKRESP_P_DESC_ID:
    sp_blockresp_bind(packet->mPayload.blockresp, newPacket);
    break;
  }
}


//Meta packet setters
void sp_setSrc(sp_packet_t* packet, uint16_t val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, packet->mPacket->mDesc);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setDst(sp_packet_t* packet, uint16_t val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, packet->mPacket->mDesc);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setCmd(sp_packet_t* packet, uint8_t val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, packet->mPacket->mDesc);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setSensora(sp_packet_t* packet, int16_t val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, packet->mPacket->mDesc);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setSensorb(sp_packet_t* packet, int val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, packet->mPacket->mDesc);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setSensorname(sp_packet_t* packet, char* val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, packet->mPacket->mDesc);
  poly_field_set(field,( const uint8_t*) val);
}

void sp_setBlockoffset(sp_packet_t* packet, uint32_t val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, packet->mPacket->mDesc);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setBlocksize(sp_packet_t* packet, uint32_t val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, packet->mPacket->mDesc);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setBlockdata(sp_packet_t* packet, uint8_t* val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, packet->mPacket->mDesc);
  poly_field_set(field,( const uint8_t*) val);
}


//Meta packet getters
uint16_t sp_getSrc(sp_packet_t* packet)
{
  uint16_t val;
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

uint16_t sp_getDst(sp_packet_t* packet)
{
  uint16_t val;
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

uint8_t sp_getCmd(sp_packet_t* packet)
{
  uint8_t val;
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

int16_t sp_getSensora(sp_packet_t* packet)
{
  int16_t val;
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

int sp_getSensorb(sp_packet_t* packet)
{
  int val;
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

char* sp_getSensorname(sp_packet_t* packet)
{
  char* val;
  val = (char*)poly_field_get(field);
  return val;
}

uint32_t sp_getBlockoffset(sp_packet_t* packet)
{
  uint32_t val;
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

uint32_t sp_getBlocksize(sp_packet_t* packet)
{
  uint32_t val;
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

uint8_t* sp_getBlockdata(sp_packet_t* packet)
{
  uint8_t* val;
  val = (uint8_t*)poly_field_get(field);
  return val;
}



/*******************************************************************************
  Packet Binding
*******************************************************************************/
/**
  *@brief Binds struct to poly_parser_t
  *@param ack ptr to ack_packet_t to be bound
  *@param packet packet to bind to
  */
void ack_bind(ack_packet_t* ack, poly_packet_t* packet)
{
  ack->mPacket = packet;


}

/**
  *@brief Binds struct to poly_parser_t
  *@param setdata ptr to setdata_packet_t to be bound
  *@param packet packet to bind to
  */
void setdata_bind(setdata_packet_t* setdata, poly_packet_t* packet)
{
  setdata->mPacket = packet;

  poly_field_bind( poly_packet_get_field(packet, SRC_F_DESC), (uint8_t*) &setdata->mSrc);
  poly_field_bind( poly_packet_get_field(packet, DST_F_DESC), (uint8_t*) &setdata->mDst);
  poly_field_bind( poly_packet_get_field(packet, SENSORA_F_DESC), (uint8_t*) &setdata->mSensora);
  poly_field_bind( poly_packet_get_field(packet, SENSORB_F_DESC), (uint8_t*) &setdata->mSensorb);
  poly_field_bind( poly_packet_get_field(packet, SENSORNAME_F_DESC), (uint8_t*) &setdata->mSensorname);

}

/**
  *@brief Binds struct to poly_parser_t
  *@param getdata ptr to getdata_packet_t to be bound
  *@param packet packet to bind to
  */
void getdata_bind(getdata_packet_t* getdata, poly_packet_t* packet)
{
  getdata->mPacket = packet;

  poly_field_bind( poly_packet_get_field(packet, SRC_F_DESC), (uint8_t*) &getdata->mSrc);
  poly_field_bind( poly_packet_get_field(packet, DST_F_DESC), (uint8_t*) &getdata->mDst);
  poly_field_bind( poly_packet_get_field(packet, SENSORA_F_DESC), (uint8_t*) &getdata->mSensora);
  poly_field_bind( poly_packet_get_field(packet, SENSORB_F_DESC), (uint8_t*) &getdata->mSensorb);
  poly_field_bind( poly_packet_get_field(packet, SENSORNAME_F_DESC), (uint8_t*) &getdata->mSensorname);

}

/**
  *@brief Binds struct to poly_parser_t
  *@param respdata ptr to respdata_packet_t to be bound
  *@param packet packet to bind to
  */
void respdata_bind(respdata_packet_t* respdata, poly_packet_t* packet)
{
  respdata->mPacket = packet;

  poly_field_bind( poly_packet_get_field(packet, SRC_F_DESC), (uint8_t*) &respdata->mSrc);
  poly_field_bind( poly_packet_get_field(packet, DST_F_DESC), (uint8_t*) &respdata->mDst);
  poly_field_bind( poly_packet_get_field(packet, SENSORA_F_DESC), (uint8_t*) &respdata->mSensora);
  poly_field_bind( poly_packet_get_field(packet, SENSORB_F_DESC), (uint8_t*) &respdata->mSensorb);
  poly_field_bind( poly_packet_get_field(packet, SENSORNAME_F_DESC), (uint8_t*) &respdata->mSensorname);

}

/**
  *@brief Binds struct to poly_parser_t
  *@param blockreq ptr to blockreq_packet_t to be bound
  *@param packet packet to bind to
  */
void blockreq_bind(blockreq_packet_t* blockreq, poly_packet_t* packet)
{
  blockreq->mPacket = packet;

  poly_field_bind( poly_packet_get_field(packet, SRC_F_DESC), (uint8_t*) &blockreq->mSrc);
  poly_field_bind( poly_packet_get_field(packet, DST_F_DESC), (uint8_t*) &blockreq->mDst);
  poly_field_bind( poly_packet_get_field(packet, BLOCKOFFSET_F_DESC), (uint8_t*) &blockreq->mBlockoffset);
  poly_field_bind( poly_packet_get_field(packet, BLOCKSIZE_F_DESC), (uint8_t*) &blockreq->mBlocksize);

}

/**
  *@brief Binds struct to poly_parser_t
  *@param blockresp ptr to blockresp_packet_t to be bound
  *@param packet packet to bind to
  */
void blockresp_bind(blockresp_packet_t* blockresp, poly_packet_t* packet)
{
  blockresp->mPacket = packet;

  poly_field_bind( poly_packet_get_field(packet, SRC_F_DESC), (uint8_t*) &blockresp->mSrc);
  poly_field_bind( poly_packet_get_field(packet, DST_F_DESC), (uint8_t*) &blockresp->mDst);
  poly_field_bind( poly_packet_get_field(packet, BLOCKOFFSET_F_DESC), (uint8_t*) &blockresp->mBlockoffset);
  poly_field_bind( poly_packet_get_field(packet, BLOCKSIZE_F_DESC), (uint8_t*) &blockresp->mBlocksize);
  poly_field_bind( poly_packet_get_field(packet, BLOCKDATA_F_DESC), (uint8_t*) &blockresp->mBlockdata);

}


/*******************************************************************************
  Weak packet handlers

  Do not modify these, just create your own without the '__weak' attribute
*******************************************************************************/
__weak uint8_t sp_ack_handler(ack_packet_t * packet)
{
  return PACKET_UNHANDLED;
}
__weak uint8_t sp_setdata_handler(setdata_packet_t * packet)
{
  return PACKET_UNHANDLED;
}
__weak uint8_t sp_getdata_handler(getdata_packet_t * packet)
{
  return PACKET_UNHANDLED;
}
__weak uint8_t sp_respdata_handler(respdata_packet_t * packet)
{
  return PACKET_UNHANDLED;
}
__weak uint8_t sp_blockreq_handler(blockreq_packet_t * packet)
{
  return PACKET_UNHANDLED;
}
__weak uint8_t sp_blockresp_handler(blockresp_packet_t * packet)
{
  return PACKET_UNHANDLED;
}
