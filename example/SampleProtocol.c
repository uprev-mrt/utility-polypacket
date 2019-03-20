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
#include <assert.h>


//Define packet IDs
#define  PP_ACK_PACKET_ID 0
#define  PP_SETDATA_PACKET_ID 1
#define  PP_GETDATA_PACKET_ID 2
#define  PP_RESPDATA_PACKET_ID 3
#define  PP_BLOCKREQ_PACKET_ID 4
#define  PP_BLOCKRESP_PACKET_ID 5


//Global descriptors
poly_packet_desc_t* PP_ACK_PACKET;
poly_packet_desc_t* PP_SETDATA_PACKET;
poly_packet_desc_t* PP_GETDATA_PACKET;
poly_packet_desc_t* PP_RESPDATA_PACKET;
poly_packet_desc_t* PP_BLOCKREQ_PACKET;
poly_packet_desc_t* PP_BLOCKRESP_PACKET;

poly_field_desc_t* PP_SRC_FIELD;
poly_field_desc_t* PP_DST_FIELD;
poly_field_desc_t* PP_CMD_FIELD;
poly_field_desc_t* PP_SENSORA_FIELD;
poly_field_desc_t* PP_SENSORB_FIELD;
poly_field_desc_t* PP_SENSORNAME_FIELD;
poly_field_desc_t* PP_BLOCKOFFSET_FIELD;
poly_field_desc_t* PP_BLOCKSIZE_FIELD;
poly_field_desc_t* PP_BLOCKDATA_FIELD;

poly_service_t* SP_SERVICE;

/*******************************************************************************
  Service Process
*******************************************************************************/
/**
  *@brief attempts to process data in buffers and parse out packets
  */
void sp_service_process()
{
  static sp_packet_t metaPacket;
  poly_packet_t* newPacket;

  HandlerStatus_e status = PACKET_UNHANDLED;

  if(poly_service_try_parse(SP_SERVICE, newPacket) == PACKET_VALID)
  {
    sp_init(&metaPacket, newPacket);

    //Dispatch packet
    switch(newPacket->mDesc->mTypeId)
    {
      case PP_ACK_PACKET_ID:
        status = sp_ack_handler(metaPacket.mPayload.ack);
        break;
      case PP_SETDATA_PACKET_ID:
        status = sp_setdata_handler(metaPacket.mPayload.setdata);
        break;
      case PP_GETDATA_PACKET_ID:
        status = sp_getdata_handler(metaPacket.mPayload.getdata);
        break;
      case PP_RESPDATA_PACKET_ID:
        status = sp_respdata_handler(metaPacket.mPayload.respdata);
        break;
      case PP_BLOCKREQ_PACKET_ID:
        status = sp_blockreq_handler(metaPacket.mPayload.blockreq);
        break;
      case PP_BLOCKRESP_PACKET_ID:
        status = sp_blockresp_handler(metaPacket.mPayload.blockresp);
        break;
      default:
        //we should never get here
        assert(false);
        break;
    }

    //If the packet was not handled, throw it to the default handler
    if(status == PACKET_UNHANDLED)
      status = sp_default_handler(&metaPacket);

    sp_teardown(&metaPacket);
  }

}

/*******************************************************************************
  Service initializer
*******************************************************************************/

/**
  *@brief initializes sp_protocol
  *@param interfaceCount number of interfaces to create
  */
void sp_service_init(int interfaceCount)
{

  SP_SERVICE = new_poly_service(6, interfaceCount);

  //Build Packet Descriptors
  PP_ACK_PACKET = new_poly_packet_desc("ack", 0);
  PP_SETDATA_PACKET = new_poly_packet_desc("SetData", 5);
  PP_GETDATA_PACKET = new_poly_packet_desc("GetData", 5);
  PP_RESPDATA_PACKET = new_poly_packet_desc("RespData", 5);
  PP_BLOCKREQ_PACKET = new_poly_packet_desc("blockReq", 4);
  PP_BLOCKRESP_PACKET = new_poly_packet_desc("blockResp", 5);

  //Build Field Descriptors
  PP_SRC_FIELD = new_poly_field_desc("src", TYPE_UINT16, 1, FORMAT_HEX);
  PP_DST_FIELD = new_poly_field_desc("dst", TYPE_UINT16, 1, FORMAT_HEX);
  PP_CMD_FIELD = new_poly_field_desc("cmd", TYPE_UINT8, 1, FORMAT_HEX);
  PP_SENSORA_FIELD = new_poly_field_desc("sensorA", TYPE_INT16, 1, FORMAT_DEC);
  PP_SENSORB_FIELD = new_poly_field_desc("sensorB", TYPE_INT, 1, FORMAT_DEC);
  PP_SENSORNAME_FIELD = new_poly_field_desc("sensorName", TYPE_STRING, 32, FORMAT_ASCII);
  PP_BLOCKOFFSET_FIELD = new_poly_field_desc("blockOffset", TYPE_UINT32, 1, FORMAT_HEX);
  PP_BLOCKSIZE_FIELD = new_poly_field_desc("blockSize", TYPE_UINT32, 1, FORMAT_DEC);
  PP_BLOCKDATA_FIELD = new_poly_field_desc("blockData", TYPE_UINT8, 64, FORMAT_NONE);

  //Settomg Field Descriptors for ack
  //Settomg Field Descriptors for SetData
  poly_packet_desc_add_field(PP_SETDATA_PACKET , PP_SRC_FIELD , true );
  poly_packet_desc_add_field(PP_SETDATA_PACKET , PP_DST_FIELD , true );
  poly_packet_desc_add_field(PP_SETDATA_PACKET , PP_SENSORA_FIELD , false );
  poly_packet_desc_add_field(PP_SETDATA_PACKET , PP_SENSORB_FIELD , false );
  poly_packet_desc_add_field(PP_SETDATA_PACKET , PP_SENSORNAME_FIELD , false );
  //Settomg Field Descriptors for GetData
  poly_packet_desc_add_field(PP_GETDATA_PACKET , PP_SRC_FIELD , true );
  poly_packet_desc_add_field(PP_GETDATA_PACKET , PP_DST_FIELD , true );
  poly_packet_desc_add_field(PP_GETDATA_PACKET , PP_SENSORA_FIELD , false );
  poly_packet_desc_add_field(PP_GETDATA_PACKET , PP_SENSORB_FIELD , false );
  poly_packet_desc_add_field(PP_GETDATA_PACKET , PP_SENSORNAME_FIELD , false );
  //Settomg Field Descriptors for RespData
  poly_packet_desc_add_field(PP_RESPDATA_PACKET , PP_SRC_FIELD , true );
  poly_packet_desc_add_field(PP_RESPDATA_PACKET , PP_DST_FIELD , true );
  poly_packet_desc_add_field(PP_RESPDATA_PACKET , PP_SENSORA_FIELD , false );
  poly_packet_desc_add_field(PP_RESPDATA_PACKET , PP_SENSORB_FIELD , false );
  poly_packet_desc_add_field(PP_RESPDATA_PACKET , PP_SENSORNAME_FIELD , false );
  //Settomg Field Descriptors for blockReq
  poly_packet_desc_add_field(PP_BLOCKREQ_PACKET , PP_SRC_FIELD , true );
  poly_packet_desc_add_field(PP_BLOCKREQ_PACKET , PP_DST_FIELD , true );
  poly_packet_desc_add_field(PP_BLOCKREQ_PACKET , PP_BLOCKOFFSET_FIELD , true );
  poly_packet_desc_add_field(PP_BLOCKREQ_PACKET , PP_BLOCKSIZE_FIELD , true );
  //Settomg Field Descriptors for blockResp
  poly_packet_desc_add_field(PP_BLOCKRESP_PACKET , PP_SRC_FIELD , true );
  poly_packet_desc_add_field(PP_BLOCKRESP_PACKET , PP_DST_FIELD , true );
  poly_packet_desc_add_field(PP_BLOCKRESP_PACKET , PP_BLOCKOFFSET_FIELD , true );
  poly_packet_desc_add_field(PP_BLOCKRESP_PACKET , PP_BLOCKSIZE_FIELD , true );
  poly_packet_desc_add_field(PP_BLOCKRESP_PACKET , PP_BLOCKDATA_FIELD , true );

  //Register packet descriptors with the service
  poly_service_register_desc(SP_SERVICE, PP_ACK_PACKET);
  poly_service_register_desc(SP_SERVICE, PP_SETDATA_PACKET);
  poly_service_register_desc(SP_SERVICE, PP_GETDATA_PACKET);
  poly_service_register_desc(SP_SERVICE, PP_RESPDATA_PACKET);
  poly_service_register_desc(SP_SERVICE, PP_BLOCKREQ_PACKET);
  poly_service_register_desc(SP_SERVICE, PP_BLOCKRESP_PACKET);

  poly_service_start(SP_SERVICE, 512);

}

void sp_service_feed(int iface, uint8_t* data, int len)
{
  poly_service_feed(SP_SERVICE,iface,data,len);
}

void sp_service_send(int iface, sp_packet_t* metaPacket)
{

}

/*******************************************************************************
  Meta packet
*******************************************************************************/

/**
  *@brief creates a new meta packet and returns a pointer to it
  *@param desc packet descriptor
  *@post creator is responsible for destroying with sp_packet_destroy()
  *@return ptr to new meta packet
  */
sp_packet_t* new_sp_packet(poly_packet_desc_t* desc)
{
  sp_packet_t* newMetaPacket = (sp_packet_t*) malloc(sizeof(sp_packet_t));

  //create new unallocated packet
  newMetaPacket->mPacket = new_poly_packet(desc, false);


  switch(newMetaPacket->mPacket->mDesc->mTypeId)
  {
    case PP_ACK_PACKET_ID:
      newMetaPacket->mPayload.ack = (ack_packet_t*) malloc(sizeof(ack_packet_t));
      sp_ack_bind(newMetaPacket->mPayload.ack, newMetaPacket->mPacket, false);
      break;
    case PP_SETDATA_PACKET_ID:
      newMetaPacket->mPayload.setdata = (setdata_packet_t*) malloc(sizeof(setdata_packet_t));
      sp_setdata_bind(newMetaPacket->mPayload.setdata, newMetaPacket->mPacket, false);
      break;
    case PP_GETDATA_PACKET_ID:
      newMetaPacket->mPayload.getdata = (getdata_packet_t*) malloc(sizeof(getdata_packet_t));
      sp_getdata_bind(newMetaPacket->mPayload.getdata, newMetaPacket->mPacket, false);
      break;
    case PP_RESPDATA_PACKET_ID:
      newMetaPacket->mPayload.respdata = (respdata_packet_t*) malloc(sizeof(respdata_packet_t));
      sp_respdata_bind(newMetaPacket->mPayload.respdata, newMetaPacket->mPacket, false);
      break;
    case PP_BLOCKREQ_PACKET_ID:
      newMetaPacket->mPayload.blockreq = (blockreq_packet_t*) malloc(sizeof(blockreq_packet_t));
      sp_blockreq_bind(newMetaPacket->mPayload.blockreq, newMetaPacket->mPacket, false);
      break;
    case PP_BLOCKRESP_PACKET_ID:
      newMetaPacket->mPayload.blockresp = (blockresp_packet_t*) malloc(sizeof(blockresp_packet_t));
      sp_blockresp_bind(newMetaPacket->mPayload.blockresp, newMetaPacket->mPacket, false);
      break;
  }

  return newMetaPacket;
}

/**
  *@brief initializes meta packet from previously parsed poly_packet_t
  *@param metaPacket ptr to metaPacket
  *@param packet ptr to packet
  */
void sp_init(sp_packet_t* metaPacket, poly_packet_t* packet)
{
  //set packet
  metaPacket->mPacket = packet;

  switch(metaPacket->mPacket->mDesc->mTypeId)
  {
    case PP_ACK_PACKET_ID:
      metaPacket->mPayload.ack = (ack_packet_t*) malloc(sizeof(ack_packet_t));
      sp_ack_bind(metaPacket->mPayload.ack, metaPacket->mPacket, true);
      break;
    case PP_SETDATA_PACKET_ID:
      metaPacket->mPayload.setdata = (setdata_packet_t*) malloc(sizeof(setdata_packet_t));
      sp_setdata_bind(metaPacket->mPayload.setdata, metaPacket->mPacket, true);
      break;
    case PP_GETDATA_PACKET_ID:
      metaPacket->mPayload.getdata = (getdata_packet_t*) malloc(sizeof(getdata_packet_t));
      sp_getdata_bind(metaPacket->mPayload.getdata, metaPacket->mPacket, true);
      break;
    case PP_RESPDATA_PACKET_ID:
      metaPacket->mPayload.respdata = (respdata_packet_t*) malloc(sizeof(respdata_packet_t));
      sp_respdata_bind(metaPacket->mPayload.respdata, metaPacket->mPacket, true);
      break;
    case PP_BLOCKREQ_PACKET_ID:
      metaPacket->mPayload.blockreq = (blockreq_packet_t*) malloc(sizeof(blockreq_packet_t));
      sp_blockreq_bind(metaPacket->mPayload.blockreq, metaPacket->mPacket, true);
      break;
    case PP_BLOCKRESP_PACKET_ID:
      metaPacket->mPayload.blockresp = (blockresp_packet_t*) malloc(sizeof(blockresp_packet_t));
      sp_blockresp_bind(metaPacket->mPayload.blockresp, metaPacket->mPacket, true);
      break;
  }
}

/**
  *@brief reset mega packet to a default state by freeing memory of payload
  *@param "metaPacket ptr to metaPacket
  */
void sp_teardown(sp_packet_t* metaPacket)
{
  switch(metaPacket->mPacket->mDesc->mTypeId)
  {
    case PP_ACK_PACKET_ID:
    free(metaPacket->mPayload.ack);
    break;
    case PP_SETDATA_PACKET_ID:
    free(metaPacket->mPayload.setdata);
    break;
    case PP_GETDATA_PACKET_ID:
    free(metaPacket->mPayload.getdata);
    break;
    case PP_RESPDATA_PACKET_ID:
    free(metaPacket->mPayload.respdata);
    break;
    case PP_BLOCKREQ_PACKET_ID:
    free(metaPacket->mPayload.blockreq);
    break;
    case PP_BLOCKRESP_PACKET_ID:
    free(metaPacket->mPayload.blockresp);
    break;
  }

  assert(metaPacket->mPacket);

  poly_packet_destroy(metaPacket->mPacket);
}

/**
  *@brief frees memory allocated for metapacket
  *@param "metaPacket ptr to metaPacket
  */
void sp_destroy(sp_packet_t* metaPacket)
{
  //teardown
  sp_teardown(metaPacket);

  //free memory
  free(metaPacket);
}

int sp_pack(sp_packet_t* metaPacket, uint8_t* data)
{
  return poly_packet_pack(metaPacket->mPacket, data);
}

ParseStatus_e sp_parse(sp_packet_t* metaPacket, uint8_t* data, int len)
{
  return poly_packet_parse_buffer(metaPacket->mPacket, data, len);
}

int sp_print_json(sp_packet_t* metaPacket, char* buf)
{
  return poly_packet_print_json(metaPacket->mPacket, buf, false);
}


/*******************************************************************************

  Meta-Packet setters

*******************************************************************************/

void sp_setSrc(sp_packet_t* packet, uint16_t val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_SRC_FIELD);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setDst(sp_packet_t* packet, uint16_t val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_DST_FIELD);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setCmd(sp_packet_t* packet, uint8_t val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_CMD_FIELD);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setSensora(sp_packet_t* packet, int16_t val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_SENSORA_FIELD);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setSensorb(sp_packet_t* packet, int val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_SENSORB_FIELD);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setSensorname(sp_packet_t* packet, const char* val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_SENSORNAME_FIELD);
  poly_field_set(field,( const uint8_t*) val);
}

void sp_setBlockoffset(sp_packet_t* packet, uint32_t val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_BLOCKOFFSET_FIELD);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setBlocksize(sp_packet_t* packet, uint32_t val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_BLOCKSIZE_FIELD);
  poly_field_set(field,( const uint8_t*) &val);
}

void sp_setBlockdata(sp_packet_t* packet, const uint8_t* val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_BLOCKDATA_FIELD);
  poly_field_set(field,( const uint8_t*) val);
}


/*******************************************************************************
  Meta-Packet getters
*******************************************************************************/

uint16_t sp_getSrc(sp_packet_t* packet)
{
  uint16_t val;
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_SRC_FIELD);
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

uint16_t sp_getDst(sp_packet_t* packet)
{
  uint16_t val;
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_DST_FIELD);
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

uint8_t sp_getCmd(sp_packet_t* packet)
{
  uint8_t val;
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_CMD_FIELD);
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

int16_t sp_getSensora(sp_packet_t* packet)
{
  int16_t val;
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_SENSORA_FIELD);
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

int sp_getSensorb(sp_packet_t* packet)
{
  int val;
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_SENSORB_FIELD);
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

char* sp_getSensorname(sp_packet_t* packet)
{
  char* val;
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_SENSORNAME_FIELD);
  val = (char*)poly_field_get(field, (uint8_t*)val);
  return val;
}

uint32_t sp_getBlockoffset(sp_packet_t* packet)
{
  uint32_t val;
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_BLOCKOFFSET_FIELD);
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

uint32_t sp_getBlocksize(sp_packet_t* packet)
{
  uint32_t val;
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_BLOCKSIZE_FIELD);
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

uint8_t* sp_getBlockdata(sp_packet_t* packet)
{
  uint8_t* val;
  poly_field_t* field = poly_packet_get_field(packet->mPacket, PP_BLOCKDATA_FIELD);
  val = (uint8_t*)poly_field_get(field, (uint8_t*)val);
  return val;
}



/*******************************************************************************
  Packet binders
*******************************************************************************/
/**
  *@brief Binds struct to poly_service_t
  *@param ack ptr to ack_packet_t to be bound
  *@param packet packet to bind to
  */
void sp_ack_bind(ack_packet_t* ack, poly_packet_t* packet, bool copy)
{
  ack->mPacket = packet;


}

/**
  *@brief Binds struct to poly_service_t
  *@param setdata ptr to setdata_packet_t to be bound
  *@param packet packet to bind to
  */
void sp_setdata_bind(setdata_packet_t* setdata, poly_packet_t* packet, bool copy)
{
  setdata->mPacket = packet;

  poly_field_bind( poly_packet_get_field(packet, PP_SRC_FIELD), (uint8_t*) &setdata->mSrc, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_DST_FIELD), (uint8_t*) &setdata->mDst, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_SENSORA_FIELD), (uint8_t*) &setdata->mSensora, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_SENSORB_FIELD), (uint8_t*) &setdata->mSensorb, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_SENSORNAME_FIELD), (uint8_t*) &setdata->mSensorname, copy);

}

/**
  *@brief Binds struct to poly_service_t
  *@param getdata ptr to getdata_packet_t to be bound
  *@param packet packet to bind to
  */
void sp_getdata_bind(getdata_packet_t* getdata, poly_packet_t* packet, bool copy)
{
  getdata->mPacket = packet;

  poly_field_bind( poly_packet_get_field(packet, PP_SRC_FIELD), (uint8_t*) &getdata->mSrc, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_DST_FIELD), (uint8_t*) &getdata->mDst, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_SENSORA_FIELD), (uint8_t*) &getdata->mSensora, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_SENSORB_FIELD), (uint8_t*) &getdata->mSensorb, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_SENSORNAME_FIELD), (uint8_t*) &getdata->mSensorname, copy);

}

/**
  *@brief Binds struct to poly_service_t
  *@param respdata ptr to respdata_packet_t to be bound
  *@param packet packet to bind to
  */
void sp_respdata_bind(respdata_packet_t* respdata, poly_packet_t* packet, bool copy)
{
  respdata->mPacket = packet;

  poly_field_bind( poly_packet_get_field(packet, PP_SRC_FIELD), (uint8_t*) &respdata->mSrc, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_DST_FIELD), (uint8_t*) &respdata->mDst, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_SENSORA_FIELD), (uint8_t*) &respdata->mSensora, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_SENSORB_FIELD), (uint8_t*) &respdata->mSensorb, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_SENSORNAME_FIELD), (uint8_t*) &respdata->mSensorname, copy);

}

/**
  *@brief Binds struct to poly_service_t
  *@param blockreq ptr to blockreq_packet_t to be bound
  *@param packet packet to bind to
  */
void sp_blockreq_bind(blockreq_packet_t* blockreq, poly_packet_t* packet, bool copy)
{
  blockreq->mPacket = packet;

  poly_field_bind( poly_packet_get_field(packet, PP_SRC_FIELD), (uint8_t*) &blockreq->mSrc, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_DST_FIELD), (uint8_t*) &blockreq->mDst, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_BLOCKOFFSET_FIELD), (uint8_t*) &blockreq->mBlockoffset, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_BLOCKSIZE_FIELD), (uint8_t*) &blockreq->mBlocksize, copy);

}

/**
  *@brief Binds struct to poly_service_t
  *@param blockresp ptr to blockresp_packet_t to be bound
  *@param packet packet to bind to
  */
void sp_blockresp_bind(blockresp_packet_t* blockresp, poly_packet_t* packet, bool copy)
{
  blockresp->mPacket = packet;

  poly_field_bind( poly_packet_get_field(packet, PP_SRC_FIELD), (uint8_t*) &blockresp->mSrc, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_DST_FIELD), (uint8_t*) &blockresp->mDst, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_BLOCKOFFSET_FIELD), (uint8_t*) &blockresp->mBlockoffset, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_BLOCKSIZE_FIELD), (uint8_t*) &blockresp->mBlocksize, copy);
  poly_field_bind( poly_packet_get_field(packet, PP_BLOCKDATA_FIELD), (uint8_t*) &blockresp->mBlockdata, copy);

}


/*******************************************************************************
  Weak packet handlers

  Do not modify these, just create your own without the '__weak' attribute
*******************************************************************************/
/**
  *@brief Handler for receiving ack packets
  *@param packet ptr to ack_packet_t  containing packet
  *@return handling status
  */
HandlerStatus_e sp_ack_handler(ack_packet_t * packet)
{
  /* NOTE : This function should not be modified, when the callback is needed,
          sp_ack_handler  should be implemented in the user file
  */
  return PACKET_UNHANDLED;
}
/**
  *@brief Handler for receiving setdata packets
  *@param packet ptr to setdata_packet_t  containing packet
  *@return handling status
  */
HandlerStatus_e sp_setdata_handler(setdata_packet_t * packet)
{
  /* NOTE : This function should not be modified, when the callback is needed,
          sp_setdata_handler  should be implemented in the user file
  */
  return PACKET_UNHANDLED;
}
/**
  *@brief Handler for receiving getdata packets
  *@param packet ptr to getdata_packet_t  containing packet
  *@return handling status
  */
HandlerStatus_e sp_getdata_handler(getdata_packet_t * packet)
{
  /* NOTE : This function should not be modified, when the callback is needed,
          sp_getdata_handler  should be implemented in the user file
  */
  return PACKET_UNHANDLED;
}
/**
  *@brief Handler for receiving respdata packets
  *@param packet ptr to respdata_packet_t  containing packet
  *@return handling status
  */
HandlerStatus_e sp_respdata_handler(respdata_packet_t * packet)
{
  /* NOTE : This function should not be modified, when the callback is needed,
          sp_respdata_handler  should be implemented in the user file
  */
  return PACKET_UNHANDLED;
}
/**
  *@brief Handler for receiving blockreq packets
  *@param packet ptr to blockreq_packet_t  containing packet
  *@return handling status
  */
HandlerStatus_e sp_blockreq_handler(blockreq_packet_t * packet)
{
  /* NOTE : This function should not be modified, when the callback is needed,
          sp_blockreq_handler  should be implemented in the user file
  */
  return PACKET_UNHANDLED;
}
/**
  *@brief Handler for receiving blockresp packets
  *@param packet ptr to blockresp_packet_t  containing packet
  *@return handling status
  */
HandlerStatus_e sp_blockresp_handler(blockresp_packet_t * packet)
{
  /* NOTE : This function should not be modified, when the callback is needed,
          sp_blockresp_handler  should be implemented in the user file
  */
  return PACKET_UNHANDLED;
}

/**
  *@brief catch-all handler for any packet not handled by its default handler
  *@param metaPacket ptr to sp_packet_t containing packet
  *@return handling status
  */
HandlerStatus_e sp_default_handler( sp_packet_t * metaPacket)
{
  /* NOTE : This function should not be modified, when the callback is needed,
          sp_default_handler  should be implemented in the user file
  */
  return PACKET_UNHANDLED;
}