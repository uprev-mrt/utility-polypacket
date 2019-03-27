/**
  *@file testService.c
  *@brief generated code for test packet service
  *@author make_protocol.py
  *@date 03/27/19
  */

/***********************************************************
        THIS FILE IS AUTOGENERATED. DO NOT MODIFY
***********************************************************/

#include "testService.h"
#include <assert.h>

//Define packet IDs
#define  TP_PACKET_PING_ID 0
#define  TP_PACKET_ACK_ID 1
#define  TP_PACKET_SENDCMD_ID 2
#define  TP_PACKET_GETDATA_ID 3
#define  TP_PACKET_DATA_ID 4


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
  TP_PACKET_PING = poly_packet_desc_init(&_TP_PACKET_PING ,"Ping", 0);
  TP_PACKET_ACK = poly_packet_desc_init(&_TP_PACKET_ACK ,"Ack", 0);
  TP_PACKET_SENDCMD = poly_packet_desc_init(&_TP_PACKET_SENDCMD ,"SendCmd", 1);
  TP_PACKET_GETDATA = poly_packet_desc_init(&_TP_PACKET_GETDATA ,"GetData", 0);
  TP_PACKET_DATA = poly_packet_desc_init(&_TP_PACKET_DATA ,"Data", 3);

  //Build Field Descriptors
  TP_FIELD_CMD = poly_field_desc_init( &_TP_FIELD_CMD ,"cmd", TYPE_UINT8, 1, FORMAT_HEX);
  TP_FIELD_SENSORA = poly_field_desc_init( &_TP_FIELD_SENSORA ,"sensorA", TYPE_INT16, 1, FORMAT_DEC);
  TP_FIELD_SENSORB = poly_field_desc_init( &_TP_FIELD_SENSORB ,"sensorB", TYPE_INT, 1, FORMAT_DEC);
  TP_FIELD_SENSORNAME = poly_field_desc_init( &_TP_FIELD_SENSORNAME ,"sensorName", TYPE_STRING, 32, FORMAT_ASCII);



  //Setting Field Descriptors for SendCmd
  poly_packet_desc_add_field(TP_PACKET_SENDCMD , TP_FIELD_CMD , true );


  //Setting Field Descriptors for Data
  poly_packet_desc_add_field(TP_PACKET_DATA , TP_FIELD_SENSORA , false );
  poly_packet_desc_add_field(TP_PACKET_DATA , TP_FIELD_SENSORB , false );
  poly_packet_desc_add_field(TP_PACKET_DATA , TP_FIELD_SENSORNAME , false );


  //Register packet descriptors with the service
  poly_service_register_desc(&TP_SERVICE, TP_PACKET_PING);
  poly_service_register_desc(&TP_SERVICE, TP_PACKET_ACK);
  poly_service_register_desc(&TP_SERVICE, TP_PACKET_SENDCMD);
  poly_service_register_desc(&TP_SERVICE, TP_PACKET_GETDATA);
  poly_service_register_desc(&TP_SERVICE, TP_PACKET_DATA);

  poly_service_start(&TP_SERVICE, 512);

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

/**
  *@brief attempts to process data in buffers and parse out packets
  */
void tp_service_process()
{
  static tp_packet_t packet;
  static tp_packet_t response;

  HandlerStatus_e status = PACKET_NOT_HANDLED;

  //reset states of static packets
  packet.mBuilt = false;
  packet.mSpooled = false;
  response.mSpooled = false;
  response.mBuilt = false;

  if(poly_service_try_parse(&TP_SERVICE, &packet.mPacket) == PACKET_VALID)
  {
    //if we get here, then the inner packet was built by the parser
    packet.mBuilt = true;

    //Dispatch packet
    switch(packet.mPacket.mDesc->mTypeId)
    {
      case TP_PACKET_PING_ID:
        tp_packet_build(&response, TP_PACKET_ACK);
        status = tp_Ping_handler(&packet, &response);
        break;
      case TP_PACKET_ACK_ID:
        status = tp_Ack_handler(&packet);
        break;
      case TP_PACKET_SENDCMD_ID:
        status = tp_SendCmd_handler(&packet);
        break;
      case TP_PACKET_GETDATA_ID:
       tp_packet_build(&response, TP_PACKET_DATA);
       status = tp_GetData_handler(&packet , &response );
        break;
      case TP_PACKET_DATA_ID:
        status = tp_Data_handler(&packet);
        break;
      default:
        //we should never get here
        assert(false);
        break;
    }

    //If this packet doe not have an explicit response and AutoAck is enabled, create an ack packet
    if(( TP_SERVICE.mAutoAck ) && (!response.mBuilt) && (!(packet.mPacket.mHeader.mToken & POLY_ACK_FLAG)))
    {
      tp_packet_build(&response, TP_PACKET_ACK);
    }

    //If the packet was not handled, throw it to the default handler
    if(status == PACKET_NOT_HANDLED)
    {
      status = tp_default_handler(&packet);
    }


    //If a response has been build and the status was not set to ignore, we send a response on the intrface it came from
    if(( status == PACKET_HANDLED) && (response.mBuilt) )
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


void tp_service_register_tx( int iface, poly_tx_callback txCallBack)
{
  poly_service_register_tx_callback(&TP_SERVICE, iface,txCallBack);
}

void tp_service_feed(int iface, uint8_t* data, int len)
{
  poly_service_feed(&TP_SERVICE,iface,data,len);
}

HandlerStatus_e tp_send(int iface, tp_packet_t* packet)
{
  HandlerStatus_e status;

  status = poly_service_spool(&TP_SERVICE, iface, &packet->mPacket);

  if(status == PACKET_SPOOLED)
  {
    packet->mSpooled = true;
  }

  return status;
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

int tp_fieldLen(tp_packet_t* packet, poly_field_desc_t* fieldDesc )
{
  poly_field_t* field = poly_packet_get_field(&packet->mPacket, TP_FIELD_SENSORNAME);
  return (int)field->mSize;
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
  poly_field_t* field = poly_packet_get_field(&packet->mPacket, TP_FIELD_CMD);
  poly_field_set(field,( const uint8_t*) &val);
}

/**
  *@brief Sets value of sensorA field
  *@param packet ptr to tp_packet
  *@param val int16_t to set field to
  */
void tp_setSensorA(tp_packet_t* packet, int16_t val)
{
  poly_field_t* field = poly_packet_get_field(&packet->mPacket, TP_FIELD_SENSORA);
  poly_field_set(field,( const uint8_t*) &val);
}

/**
  *@brief Sets value of sensorB field
  *@param packet ptr to tp_packet
  *@param val int to set field to
  */
void tp_setSensorB(tp_packet_t* packet, int val)
{
  poly_field_t* field = poly_packet_get_field(&packet->mPacket, TP_FIELD_SENSORB);
  poly_field_set(field,( const uint8_t*) &val);
}

/**
  *@brief Sets value(s) in sensorName field
  *@param packet ptr to tp_packet
  *@param val char* to copy data from
  */
void tp_setSensorName(tp_packet_t* packet, const char* val)
{
  poly_field_t* field = poly_packet_get_field(&packet->mPacket, TP_FIELD_SENSORNAME);
  poly_field_set(field,( const uint8_t*) val);
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
  poly_field_t* field = poly_packet_get_field(&packet->mPacket, TP_FIELD_CMD);
  poly_field_get(field,(uint8_t*) &val);
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
  poly_field_t* field = poly_packet_get_field(&packet->mPacket, TP_FIELD_SENSORA);
  poly_field_get(field,(uint8_t*) &val);
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
  poly_field_t* field = poly_packet_get_field(&packet->mPacket, TP_FIELD_SENSORB);
  poly_field_get(field,(uint8_t*) &val);
  return val;
}

/**
  *@brief Gets value of sensorName field
  *@param packet ptr to tp_packet
  *@return char* of data in field
  */
char* tp_getSensorName(tp_packet_t* packet)
{
  char* val;
  poly_field_t* field = poly_packet_get_field(&packet->mPacket, TP_FIELD_SENSORNAME);
  val = (char*)poly_field_get(field, NULL);
  return val;
}


/*******************************************************************************
  Quick send functions
*******************************************************************************/

HandlerStatus_e tp_sendPing(int iface)
{
  HandlerStatus_e status;
  //create packet
  tp_packet_t packet;
  tp_packet_build(&packet, TP_PACKET_PING);

  status = tp_send(iface,&packet); //send packet
  tp_clean(&packet); //This will only free the underlying packet if the spooling was unsuccessful
  return status;
}

/**
  *@brief sends SendCmd packet
  *@param iface indec of interface to send packet to
  *@param cmd value to set cmd field to
  *@return status send attempt
  */
HandlerStatus_e tp_sendSendCmd(int iface, uint8_t cmd)
{
  HandlerStatus_e status;
  //create packet
  tp_packet_t packet;
  tp_packet_build(&packet,TP_PACKET_SENDCMD);

  //set fields
  tp_setCmd(&packet, cmd);

  status = tp_send(iface,&packet); //send packet
  tp_clean(&packet); //This will only free the underlying packet if the spooling was unsuccessful
  return status;
}
/**
  *@brief sends GetData packet
  *@param iface indec of interface to send packet to
  *@return status send attempt
  */
HandlerStatus_e tp_sendGetData(int iface)
{
  HandlerStatus_e status;
  //create packet
  tp_packet_t packet;
  tp_packet_build(&packet,TP_PACKET_GETDATA);

  //set fields

  status = tp_send(iface,&packet); //send packet
  tp_clean(&packet); //This will only free the underlying packet if the spooling was unsuccessful
  return status;
}
/**
  *@brief sends Data packet
  *@param iface indec of interface to send packet to
  *@param sensorA value to set sensorA field to
  *@param sensorB value to set sensorB field to
  *@param sensorName char* to sensorName field from
  *@return status send attempt
  */
HandlerStatus_e tp_sendData(int iface, int16_t sensorA, int sensorB, const char* sensorName)
{
  HandlerStatus_e status;
  //create packet
  tp_packet_t packet;
  tp_packet_build(&packet,TP_PACKET_DATA);

  //set fields
  tp_setSensorA(&packet, sensorA);
  tp_setSensorB(&packet, sensorB);
  tp_setSensorName(&packet, sensorName);

  status = tp_send(iface,&packet); //send packet
  tp_clean(&packet); //This will only free the underlying packet if the spooling was unsuccessful
  return status;
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
  *@return handling status
  */
__attribute__((weak)) HandlerStatus_e tp_SendCmd_handler(tp_packet_t* tp_SendCmd)
{
  /*  Get Required Fields in packet */
  //uint8_t cmd = tp_getCmd(tp_SendCmd); //


  /* NOTE : This function should not be modified! If needed,  It should be overridden in the application code */

  return PACKET_NOT_HANDLED;
}

/**
  *@brief Handler for receiving GetData packets
  *@param GetData incoming GetData packet
  *@param Data Data packet to respond with
  *@return handling status
  */
__attribute__((weak)) HandlerStatus_e tp_GetData_handler(tp_packet_t* tp_GetData, tp_packet_t* tp_Data)
{
  /*  Get Required Fields in packet */
  /*    Set required Fields in response  */


  /* NOTE : This function should not be modified! If needed,  It should be overridden in the application code */

  return PACKET_NOT_HANDLED;
}

/**
  *@brief Handler for receiving Data packets
  *@param Data incoming Data packet
  *@return handling status
  */
__attribute__((weak)) HandlerStatus_e tp_Data_handler(tp_packet_t* tp_Data)
{
  /*  Get Required Fields in packet */


  /* NOTE : This function should not be modified! If needed,  It should be overridden in the application code */

  return PACKET_NOT_HANDLED;
}


/**
  *@brief catch-all handler for any packet not handled by its default handler
  *@param metaPacket ptr to tp_packet_t containing packet
  *@return handling status
  */
__attribute__((weak)) HandlerStatus_e tp_default_handler( tp_packet_t * tp_packet)
{

  /* NOTE : This function should not be modified, when the callback is needed,
          tp_default_handler  should be implemented in the user file
  */

  return PACKET_HANDLED;
}
