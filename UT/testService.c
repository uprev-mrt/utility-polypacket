/**
  *@file testService.c
  *@brief generated code for test packet service
  *@author make_protocol.py
  *@date 09/05/19
  */

/***********************************************************
        THIS FILE IS AUTOGENERATED. DO NOT MODIFY
***********************************************************/

#include "testService.h"
#include <assert.h>

//Define packet IDs
#define TP_PACKET_PING_ID 0
#define TP_PACKET_ACK_ID 1
#define TP_PACKET_SENDCMD_ID 2
#define TP_PACKET_GETDATA_ID 3
#define TP_PACKET_DATA_ID 4



//Global descriptors
poly_packet_desc_t* TP_PACKET_PING;
poly_packet_desc_t* TP_PACKET_ACK;
poly_packet_desc_t* TP_PACKET_SENDCMD;
poly_packet_desc_t* TP_PACKET_GETDATA;
poly_packet_desc_t* TP_PACKET_DATA;


poly_field_desc_t* TP_FIELD_CMD;
poly_field_desc_t* TP_FIELD_SENSORA;
poly_field_desc_t* TP_FIELD_SENSORB;
poly_field_desc_t* TP_FIELD_SENSORNAME;

//Global descriptors
poly_packet_desc_t _TP_PACKET_PING;
poly_packet_desc_t _TP_PACKET_ACK;
poly_packet_desc_t _TP_PACKET_SENDCMD;
poly_packet_desc_t _TP_PACKET_GETDATA;
poly_packet_desc_t _TP_PACKET_DATA;


poly_field_desc_t _TP_FIELD_CMD;
poly_field_desc_t _TP_FIELD_SENSORA;
poly_field_desc_t _TP_FIELD_SENSORB;
poly_field_desc_t _TP_FIELD_SENSORNAME;

static poly_service_t TP_SERVICE;

/*******************************************************************************
  Service Functions
*******************************************************************************/

/**
  *@brief initializes tp_protocol
  *@param interfaceCount number of interfaces to create
  */
void tp_service_init(int interfaceCount)
{
  //initialize core service
  poly_service_init(&TP_SERVICE,5, interfaceCount);

  //Build Packet Descriptors
  TP_PACKET_PING = poly_packet_desc_init(&_TP_PACKET_PING ,TP_PACKET_PING_ID,"Ping", 0);
  TP_PACKET_ACK = poly_packet_desc_init(&_TP_PACKET_ACK ,TP_PACKET_ACK_ID,"Ack", 0);
  TP_PACKET_SENDCMD = poly_packet_desc_init(&_TP_PACKET_SENDCMD ,TP_PACKET_SENDCMD_ID,"SendCmd", 1);
  TP_PACKET_GETDATA = poly_packet_desc_init(&_TP_PACKET_GETDATA ,TP_PACKET_GETDATA_ID,"GetData", 0);
  TP_PACKET_DATA = poly_packet_desc_init(&_TP_PACKET_DATA ,TP_PACKET_DATA_ID,"Data", 3);

  //Build Struct Descriptors

  //Build Field Descriptors
  TP_FIELD_CMD = poly_field_desc_init( &_TP_FIELD_CMD ,"cmd", TYPE_UINT8, 1, FORMAT_HEX);
  TP_FIELD_SENSORA = poly_field_desc_init( &_TP_FIELD_SENSORA ,"sensorA", TYPE_INT16, 1, FORMAT_HEX);
  TP_FIELD_SENSORB = poly_field_desc_init( &_TP_FIELD_SENSORB ,"sensorB", TYPE_INT, 1, FORMAT_DEC);
  TP_FIELD_SENSORNAME = poly_field_desc_init( &_TP_FIELD_SENSORNAME ,"sensorName", TYPE_STRING, 32, FORMAT_ASCII);



  //Setting Field Descriptors for packet: SendCmd
  poly_packet_desc_add_field(TP_PACKET_SENDCMD , TP_FIELD_CMD , true );


  //Setting Field Descriptors for packet: Data
  poly_packet_desc_add_field(TP_PACKET_DATA , TP_FIELD_SENSORA , false );
  poly_packet_desc_add_field(TP_PACKET_DATA , TP_FIELD_SENSORB , false );
  poly_packet_desc_add_field(TP_PACKET_DATA , TP_FIELD_SENSORNAME , false );



  //Register packet descriptors with the service
  poly_service_register_desc(&TP_SERVICE, TP_PACKET_PING);
  poly_service_register_desc(&TP_SERVICE, TP_PACKET_ACK);
  poly_service_register_desc(&TP_SERVICE, TP_PACKET_SENDCMD);
  poly_service_register_desc(&TP_SERVICE, TP_PACKET_GETDATA);
  poly_service_register_desc(&TP_SERVICE, TP_PACKET_DATA);

  //Register packet descriptors with the service

  poly_service_start(&TP_SERVICE, 16);

}


void tp_service_teardown()
{
  //deinit Packet Descriptors
  TP_PACKET_PING = poly_packet_desc_deinit(&_TP_PACKET_PING);
  TP_PACKET_ACK = poly_packet_desc_deinit(&_TP_PACKET_ACK);
  TP_PACKET_SENDCMD = poly_packet_desc_deinit(&_TP_PACKET_SENDCMD);
  TP_PACKET_GETDATA = poly_packet_desc_deinit(&_TP_PACKET_GETDATA);
  TP_PACKET_DATA = poly_packet_desc_deinit(&_TP_PACKET_DATA);

  //deinitialize core service
  poly_service_deinit(&TP_SERVICE);

}

HandlerStatus_e tp_service_dispatch(tp_packet_t* packet, tp_packet_t* response)
{

  HandlerStatus_e tp_status;

  //Dispatch packet
  switch(packet->mPacket.mDesc->mTypeId)
  {
    case TP_PACKET_PING_ID:
      tp_packet_build(response, TP_PACKET_ACK);
      tp_status = tp_Ping_handler(packet, response);
      break;
    case TP_PACKET_ACK_ID:
      tp_status = tp_Ack_handler(packet);
      break;
    case TP_PACKET_SENDCMD_ID:
      tp_status = tp_SendCmd_handler(packet);
      break;
    case TP_PACKET_GETDATA_ID:
     tp_packet_build(response, TP_PACKET_DATA);
     tp_status = tp_GetData_handler(packet , response );
      break;
    case TP_PACKET_DATA_ID:
      tp_status = tp_Data_handler(packet);
      break;
    default:
      //we should never get here
      assert(false);
      break;
  }

  //If this packet doe not have an explicit response and AutoAck is enabled, create an ack packet
  if(( TP_SERVICE.mAutoAck ) && (!response->mBuilt) && (!(packet->mPacket.mHeader.mToken & POLY_ACK_FLAG)))
  {
    tp_packet_build(response, TP_PACKET_ACK);
  }

  //If the packet was not handled, throw it to the default handler
  if(tp_status == PACKET_NOT_HANDLED)
  {
    tp_status = tp_default_handler(packet);
  }

  return tp_status;
}

/**
  *@brief attempts to process data in buffers and parse out packets
  */
void tp_service_process()
{
  static tp_packet_t packet;
  static tp_packet_t response;

  HandlerStatus_e tp_status = PACKET_NOT_HANDLED;

  //reset states of static packets
  packet.mBuilt = false;
  packet.mSpooled = false;
  response.mSpooled = false;
  response.mBuilt = false;

  if(poly_service_try_parse(&TP_SERVICE, &packet.mPacket) == PACKET_VALID)
  {
    //if we get here, then the inner packet was built by the parser
    packet.mBuilt = true;

    tp_status = tp_service_dispatch(&packet,&response);

    //If a response has been build and the tp_status was not set to ignore, we send a response on the intrface it came from
    if(( tp_status == PACKET_HANDLED) && (response.mBuilt) )
    {
      //set response token with ack flag
			response.mPacket.mHeader.mToken = packet.mPacket.mHeader.mToken | POLY_ACK_FLAG;

      tp_send(packet.mPacket.mInterface , &response);
    }

    //Clean the packets
    tp_clean(&packet);
    tp_clean(&response);
  }

  //despool any packets ready to go out
  poly_service_despool(&TP_SERVICE);

}


void tp_service_register_bytes_tx( int iface, poly_tx_bytes_callback txBytesCallBack)
{
  poly_service_register_bytes_tx_callback(&TP_SERVICE, iface,txBytesCallBack);
}

void tp_service_register_packet_tx( int iface, poly_tx_packet_callback txPacketCallBack)
{
  poly_service_register_packet_tx_callback(&TP_SERVICE, iface,txPacketCallBack);
}

void tp_service_feed(int iface, uint8_t* data, int len)
{
  poly_service_feed(&TP_SERVICE,iface,data,len);
}

HandlerStatus_e tp_handle_json(const char* req, int len, char* resp)
{
  tp_packet_t packet;
  tp_packet_t response;

  //reset states of static packets
  HandlerStatus_e tp_status = PACKET_NOT_HANDLED;
  packet.mBuilt = false;
  packet.mSpooled = false;
  response.mSpooled = false;
  response.mBuilt = false;


  if(poly_service_parse_json(&TP_SERVICE, &packet.mPacket, req, len) == PACKET_VALID)
  {
    //if we get here, then the inner packet was built by the parser
    packet.mBuilt = true;

    tp_status = tp_service_dispatch(&packet,&response);


    //If a response has been build and the tp_status was not set to ignore, we send a response on the intrface it came from
    if(( tp_status == PACKET_HANDLED) && (response.mBuilt) )
    {
      //set response token with ack flag
			response.mPacket.mHeader.mToken = packet.mPacket.mHeader.mToken | POLY_ACK_FLAG;
      poly_packet_print_json(&response.mPacket, resp, false);
    }

    //Clean the packets
    tp_clean(&packet);
    tp_clean(&response);

  }
  else
  {
    MRT_SPRINTF(resp,"{\"Error\":\"Parsing Error\"}");
  }

  return tp_status;
}


void tp_service_feed_json(int iface, const char* msg, int len)
{
  poly_service_feed_json_msg(&TP_SERVICE,iface,msg,len);
}

HandlerStatus_e tp_send(int iface, tp_packet_t* packet)
{
  HandlerStatus_e tp_status;

  tp_status = poly_service_spool(&TP_SERVICE, iface, &packet->mPacket);

  if(tp_status == PACKET_SPOOLED)
  {
    packet->mSpooled = true;
  }

  return tp_status;
}

void tp_auto_ack(bool enable)
{
  TP_SERVICE.mAutoAck = enable;
}


/*******************************************************************************
  Meta packet
*******************************************************************************/

/**
  *@brief initializes a new {proto.prefix}_packet_t
  *@param desc ptr to packet descriptor to model packet from
  */
void tp_packet_build(tp_packet_t* packet, poly_packet_desc_t* desc)
{
  //create new allocated packet
  poly_packet_build(&packet->mPacket, desc, true);
  packet->mBuilt = true;
  packet->mSpooled = false;
}


/**
  *@brief frees memory allocated for metapacket
  *@param packet ptr to metaPacket
  *
  */
void tp_clean(tp_packet_t* packet)
{
  //If the packet has been spooled, the spool is responsible for it now
  if(packet->mBuilt && (!packet->mSpooled))
  {
    poly_packet_clean(&packet->mPacket);
  }

}



/*******************************************************************************

  Meta-Packet setters

*******************************************************************************/

/**
  *@brief Sets value of cmd field
  *@param packet ptr to tp_packet
  *@param val uint8_t to set field to
  */
void tp_setCmd(tp_packet_t* packet, uint8_t val)
{
  poly_packet_set_field(&packet->mPacket, TP_FIELD_CMD, &val);
}

/**
  *@brief Sets value of sensorA field
  *@param packet ptr to tp_packet
  *@param val int16_t to set field to
  */
void tp_setSensorA(tp_packet_t* packet, int16_t val)
{
  poly_packet_set_field(&packet->mPacket, TP_FIELD_SENSORA, &val);
}

/**
  *@brief Sets value of sensorB field
  *@param packet ptr to tp_packet
  *@param val int to set field to
  */
void tp_setSensorB(tp_packet_t* packet, int val)
{
  poly_packet_set_field(&packet->mPacket, TP_FIELD_SENSORB, &val);
}

/**
  *@brief Sets value(s) in sensorName field
  *@param packet ptr to tp_packet
  *@param val char* to copy data from
  */
void tp_setSensorName(tp_packet_t* packet, const char* val)
{
  poly_packet_set_field(&packet->mPacket, TP_FIELD_SENSORNAME, val);
}


/*******************************************************************************
  Meta-Packet getters
*******************************************************************************/

/**
  *@brief Gets value of cmd field
  *@param packet ptr to tp_packet
  *@return uint8_t data from field
  */
uint8_t tp_getCmd(tp_packet_t* packet)
{
  uint8_t val;
  poly_packet_get_field(&packet->mPacket, TP_FIELD_CMD, &val);
  return val;
}

/**
  *@brief Gets value of sensorA field
  *@param packet ptr to tp_packet
  *@return int16_t data from field
  */
int16_t tp_getSensorA(tp_packet_t* packet)
{
  int16_t val;
  poly_packet_get_field(&packet->mPacket, TP_FIELD_SENSORA, &val);
  return val;
}

/**
  *@brief Gets value of sensorB field
  *@param packet ptr to tp_packet
  *@return int data from field
  */
int tp_getSensorB(tp_packet_t* packet)
{
  int val;
  poly_packet_get_field(&packet->mPacket, TP_FIELD_SENSORB, &val);
  return val;
}

/**
  *@brief Gets value of sensorName field
  *@param packet ptr to tp_packet
  *@return char* of data in field
  */
void tp_getSensorName(tp_packet_t* packet, char* val)
{
  poly_packet_get_field(&packet->mPacket, TP_FIELD_SENSORNAME, val);
}


/*******************************************************************************
  Quick send functions
*******************************************************************************/

HandlerStatus_e tp_sendPing(int iface)
{
  HandlerStatus_e tp_status;
  //create packet
  tp_packet_t packet;
  tp_packet_build(&packet, TP_PACKET_PING);

  tp_status = tp_send(iface,&packet); //send packet
  tp_clean(&packet); //This will only free the underlying packet if the spooling was unsuccessful
  return tp_status;
}

/**
  *@brief sends SendCmd packet
  *@param iface indec of interface to send packet to
  *@param cmd value to set cmd field to
  *@return tp_status send attempt
  */
HandlerStatus_e tp_sendSendCmd(int iface, uint8_t cmd)
{
  HandlerStatus_e tp_status;
  //create packet
  tp_packet_t packet;
  tp_packet_build(&packet,TP_PACKET_SENDCMD);

  //set fields
  tp_setCmd(&packet, cmd);

  tp_status = tp_send(iface,&packet); //send packet
  tp_clean(&packet); //This will only free the underlying packet if the spooling was unsuccessful
  return tp_status;
}
/**
  *@brief sends GetData packet
  *@param iface indec of interface to send packet to
  *@return tp_status send attempt
  */
HandlerStatus_e tp_sendGetData(int iface)
{
  HandlerStatus_e tp_status;
  //create packet
  tp_packet_t packet;
  tp_packet_build(&packet,TP_PACKET_GETDATA);

  //set fields

  tp_status = tp_send(iface,&packet); //send packet
  tp_clean(&packet); //This will only free the underlying packet if the spooling was unsuccessful
  return tp_status;
}
/**
  *@brief sends Data packet
  *@param iface indec of interface to send packet to
  *@param sensorA value to set sensorA field to
  *@param sensorB value to set sensorB field to
  *@param sensorName char* to sensorName field from
  *@return tp_status send attempt
  */
HandlerStatus_e tp_sendData(int iface, int16_t sensorA, int sensorB, const char* sensorName)
{
  HandlerStatus_e tp_status;
  //create packet
  tp_packet_t packet;
  tp_packet_build(&packet,TP_PACKET_DATA);

  //set fields
  tp_setSensorA(&packet, sensorA);
  tp_setSensorB(&packet, sensorB);
  tp_setSensorName(&packet, sensorName);

  tp_status = tp_send(iface,&packet); //send packet
  tp_clean(&packet); //This will only free the underlying packet if the spooling was unsuccessful
  return tp_status;
}


/*******************************************************************************
  Weak packet handlers

  Do not modify these, just create your own without the '__weak' attribute
*******************************************************************************/
/**
  *@brief Handler for receiving ping packets
  *@param tp_ping ptr to incoming ping packet
  *@param tp_ack ptr to repsonding ack
  *@return PACKET_HANDLED
  */
__attribute__((weak)) HandlerStatus_e tp_Ping_handler(tp_packet_t* tp_ping, tp_packet_t* tp_ack)
{
  /* Ack token has already been set as ping token with POLY_ACK_FLAG*/
  return PACKET_HANDLED;
}

/**
  *@brief Handler for receiving ack packets
  *@param tp_ack ptr to ack
  *@return PACKET_HANDLED
  */
__attribute__((weak)) HandlerStatus_e tp_Ack_handler(tp_packet_t* tp_ack)
{
  return PACKET_HANDLED;
}

/**
  *@brief Handler for receiving SendCmd packets
  *@param SendCmd incoming SendCmd packet
  *@return handling tp_status
  */
__attribute__((weak)) HandlerStatus_e tp_SendCmd_handler(tp_packet_t* tp_SendCmd)
{
  /*  Get Required Fields in packet */
  //uint8_t cmd;  //

  //cmd = tp_getCmd(tp_SendCmd);


  /* NOTE : This function should not be modified! If needed,  It should be overridden in the application code */

  return PACKET_NOT_HANDLED;
}

/**
  *@brief Handler for receiving GetData packets
  *@param GetData incoming GetData packet
  *@param Data Data packet to respond with
  *@return handling tp_status
  */
__attribute__((weak)) HandlerStatus_e tp_GetData_handler(tp_packet_t* tp_GetData, tp_packet_t* tp_Data)
{
  /*  Get Required Fields in packet */

  /*    Set required Fields in response  */
  //tp_setSensorA(tp_Data, value );  //Value of Sensor A
  //tp_setSensorB(tp_Data, value );  //Value of Sensor B
  //tp_setSensorName(tp_Data, value );  //Name of sensor responding to message


  /* NOTE : This function should not be modified! If needed,  It should be overridden in the application code */

  return PACKET_NOT_HANDLED;
}

/**
  *@brief Handler for receiving Data packets
  *@param Data incoming Data packet
  *@return handling tp_status
  */
__attribute__((weak)) HandlerStatus_e tp_Data_handler(tp_packet_t* tp_Data)
{
  /*  Get Required Fields in packet */
  //int16_t sensorA;  //Value of Sensor A
  //int sensorB;  //Value of Sensor B
  //char sensorName[32];  //Name of sensor responding to message

  //sensorA = tp_getSensorA(tp_Data);
  //sensorB = tp_getSensorB(tp_Data);
  //tp_getSensorName(tp_Data, sensorName);


  /* NOTE : This function should not be modified! If needed,  It should be overridden in the application code */

  return PACKET_NOT_HANDLED;
}
