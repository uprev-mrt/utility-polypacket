/**
  *@file ${proto.fileName}.c
  *@brief generated code for ${proto.name} packet service
  *@author make_protocol.py
  *@date ${proto.genTime}
  */

/***********************************************************
        THIS FILE IS AUTOGENERATED. DO NOT MODIFY
***********************************************************/

#include "${proto.fileName}.h"
#include <assert.h>

//Define packet IDs
% for packet in proto.packets:
#define ${packet.globalName}_ID ${packet.packetId}
% endfor


//Global descriptors
% for packet in proto.packets:
poly_packet_desc_t* ${packet.globalName};
% endfor

% for field in proto.fields:
poly_field_desc_t* ${field.globalName};
% endfor

//Global descriptors
% for packet in proto.packets:
poly_packet_desc_t _${packet.globalName};
% endfor

% for field in proto.fields:
poly_field_desc_t _${field.globalName};
% endfor

static poly_service_t ${proto.service()};

/*******************************************************************************
  Service Functions
*******************************************************************************/

/**
  *@brief initializes ${proto.prefix}_protocol
  *@param interfaceCount number of interfaces to create
  */
void ${proto.prefix}_service_init(int interfaceCount)
{
  //initialize core service
  poly_service_init(&${proto.service()},${len(proto.packets)}, interfaceCount);

  //Build Packet Descriptors
% for packet in proto.packets:
  ${packet.globalName} = poly_packet_desc_init(&_${packet.globalName} ,${packet.globalName}_ID,"${packet.name}", ${len(packet.fields)});
% endfor

  //Build Field Descriptors
% for field in proto.fields:
  ${field.globalName} = poly_field_desc_init( &_${field.globalName} ,"${field.name}", TYPE_${field.type.upper()}, ${field.arrayLen}, ${field.format.upper()});
% endfor

% for packet in proto.packets:
% if len(packet.fields) > 0:
  //Setting Field Descriptors for ${packet.name}
  % for field in packet.fields:
  poly_packet_desc_add_field(${packet.globalName} , ${field.globalName} , ${str(field.isRequired).lower()} );
  % endfor
% endif

% endfor

  //Register packet descriptors with the service
% for packet in proto.packets:
  poly_service_register_desc(&${proto.service()}, ${packet.globalName});
% endfor

  poly_service_start(&${proto.service()}, 16);

}


void ${proto.prefix}_service_teardown()
{
  //deinit Packet Descriptors
% for packet in proto.packets:
  ${packet.globalName} = poly_packet_desc_deinit(&_${packet.globalName});
% endfor

  //deinitialize core service
  poly_service_deinit(&${proto.service()});

}

/**
  *@brief attempts to process data in buffers and parse out packets
  */
void ${proto.prefix}_service_process()
{
  static ${proto.prefix}_packet_t packet;
  static ${proto.prefix}_packet_t response;

  HandlerStatus_e status = PACKET_NOT_HANDLED;

  //reset states of static packets
  packet.mBuilt = false;
  packet.mSpooled = false;
  response.mSpooled = false;
  response.mBuilt = false;

  if(poly_service_try_parse(&${proto.service()}, &packet.mPacket) == PACKET_VALID)
  {
    //if we get here, then the inner packet was built by the parser
    packet.mBuilt = true;

    //Dispatch packet
    switch(packet.mPacket.mDesc->mTypeId)
    {
      case ${proto.prefix.upper()}_PACKET_PING_ID:
        ${proto.prefix}_packet_build(&response, ${proto.prefix.upper()}_PACKET_ACK);
        status = ${proto.prefix}_Ping_handler(&packet, &response);
        break;
      case ${proto.prefix.upper()}_PACKET_ACK_ID:
        status = ${proto.prefix}_Ack_handler(&packet);
        break;
  % for packet in proto.packets:
  % if not packet.standard:
      case ${packet.globalName}_ID:
      %if packet.hasResponse:
       ${proto.prefix}_packet_build(&response, ${packet.response.globalName});
       status = ${proto.prefix}_${packet.camel()}_handler(&packet , &response );
      %else:
        status = ${proto.prefix}_${packet.camel()}_handler(&packet);
      %endif
        break;
  % endif
  % endfor
      default:
        //we should never get here
        assert(false);
        break;
    }

    //If this packet doe not have an explicit response and AutoAck is enabled, create an ack packet
    if(( ${proto.prefix.upper()}_SERVICE.mAutoAck ) && (!response.mBuilt) && (!(packet.mPacket.mHeader.mToken & POLY_ACK_FLAG)))
    {
      ${proto.prefix}_packet_build(&response, ${proto.prefix.upper()}_PACKET_ACK);
    }

    //If the packet was not handled, throw it to the default handler
    if(status == PACKET_NOT_HANDLED)
    {
      status = ${proto.prefix}_default_handler(&packet);
    }


    //If a response has been build and the status was not set to ignore, we send a response on the intrface it came from
    if(( status == PACKET_HANDLED) && (response.mBuilt) )
    {
      //set response token with ack flag
			response.mPacket.mHeader.mToken = packet.mPacket.mHeader.mToken | POLY_ACK_FLAG;

      ${proto.prefix}_send(packet.mPacket.mInterface , &response);
    }

    //Clean the packets
    ${proto.prefix}_clean(&packet);
    ${proto.prefix}_clean(&response);
  }

  //despool any packets ready to go out
  poly_service_despool(&${proto.service()});

}


void ${proto.prefix}_service_register_tx( int iface, poly_tx_callback txCallBack)
{
  poly_service_register_tx_callback(&${proto.service()}, iface,txCallBack);
}

void ${proto.prefix}_service_feed(int iface, uint8_t* data, int len)
{
  poly_service_feed(&${proto.service()},iface,data,len);
}

HandlerStatus_e ${proto.prefix}_send(int iface, ${proto.prefix}_packet_t* packet)
{
  HandlerStatus_e status;

  status = poly_service_spool(&${proto.service()}, iface, &packet->mPacket);

  if(status == PACKET_SPOOLED)
  {
    packet->mSpooled = true;
  }

  return status;
}

void ${proto.prefix}_auto_ack(bool enable)
{
  ${proto.service()}.mAutoAck = enable;
}


/*******************************************************************************
  Meta packet
*******************************************************************************/

/**
  *@brief initializes a new {proto.prefix}_packet_t
  *@param desc ptr to packet descriptor to model packet from
  */
void ${proto.prefix}_packet_build(${proto.prefix}_packet_t* packet, poly_packet_desc_t* desc)
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
void ${proto.prefix}_clean(${proto.prefix}_packet_t* packet)
{
  //If the packet has been spooled, the spool is responsible for it now
  if(packet->mBuilt && (!packet->mSpooled))
  {
    poly_packet_clean(&packet->mPacket);
  }

}

int ${proto.prefix}_fieldLen(${proto.prefix}_packet_t* packet, poly_field_desc_t* fieldDesc )
{
  poly_field_t* field = poly_packet_get_field(&packet->mPacket, ${field.globalName});
  return (int)field->mSize;
}

/*******************************************************************************

  Meta-Packet setters

*******************************************************************************/

% for field in proto.fields:
%if field.isArray:
/**
  *@brief Sets value(s) in ${field.name} field
  *@param packet ptr to ${proto.prefix}_packet
  *@param val ${field.getParamType()} to copy data from
  */
void ${proto.prefix}_set${field.camel()}(${proto.prefix}_packet_t* packet, const ${field.getParamType()} val)
% else:
/**
  *@brief Sets value of ${field.name} field
  *@param packet ptr to ${proto.prefix}_packet
  *@param val ${field.getParamType()} to set field to
  */
void ${proto.prefix}_set${field.camel()}(${proto.prefix}_packet_t* packet, ${field.getParamType()} val)
%endif
{
  poly_field_t* field = poly_packet_get_field(&packet->mPacket, ${field.globalName});
%if field.isArray:
  poly_field_set(field,( const uint8_t*) val);
% else:
  poly_field_set(field,( const uint8_t*) &val);
% endif
}

% endfor

/*******************************************************************************
  Meta-Packet getters
*******************************************************************************/

% for field in proto.fields:
%if field.isArray:
/**
  *@brief Gets value of ${field.name} field
  *@param packet ptr to ${proto.prefix}_packet
  *@return ${field.getParamType()} of data in field
  */
%else:
/**
  *@brief Gets value of ${field.name} field
  *@param packet ptr to ${proto.prefix}_packet
  *@return ${field.getParamType()} data from field
  */
%endif
${field.getParamType()} ${proto.prefix}_get${field.camel()}(${proto.prefix}_packet_t* packet)
{
  ${field.getParamType()} val;
  poly_field_t* field = poly_packet_get_field(&packet->mPacket, ${field.globalName});
%if field.isArray:
  val = (${field.getParamType()})poly_field_get(field, NULL);
% else:
  poly_field_get(field,(uint8_t*) &val);
% endif
  return val;
}

% endfor

/*******************************************************************************
  Quick send functions
*******************************************************************************/

HandlerStatus_e ${proto.prefix}_sendPing(int iface)
{
  HandlerStatus_e status;
  //create packet
  ${proto.prefix}_packet_t packet;
  ${proto.prefix}_packet_build(&packet, ${proto.prefix.upper()}_PACKET_PING);

  status = ${proto.prefix}_send(iface,&packet); //send packet
  ${proto.prefix}_clean(&packet); //This will only free the underlying packet if the spooling was unsuccessful
  return status;
}

% for packet in proto.packets:
%if not packet.standard:
/**
  *@brief sends ${packet.name} packet
  *@param iface indec of interface to send packet to
  %for field in packet.fields:
  %if field.isArray:
  *@param ${field.name} ${field.getParamType()} to ${field.name} field from
  %else:
  *@param ${field.name} value to set ${field.name} field to
  %endif
  %endfor
  *@return status send attempt
  */
HandlerStatus_e ${proto.prefix}_send${packet.camel()}(int iface\
  %for idx,field in enumerate(packet.fields):
,\
  %if field.isArray:
 const ${field.getParamType()} ${field.name}\
  %else:
 ${field.getParamType()} ${field.name}\
  %endif
  %endfor
)
{
  HandlerStatus_e status;
  //create packet
  ${proto.prefix}_packet_t packet;
  ${proto.prefix}_packet_build(&packet,${packet.globalName});

  //set fields
  %for field in packet.fields:
  ${proto.prefix}_set${field.camel()}(&packet, ${field.name});
  %endfor

  status = ${proto.prefix}_send(iface,&packet); //send packet
  ${proto.prefix}_clean(&packet); //This will only free the underlying packet if the spooling was unsuccessful
  return status;
}
%endif
% endfor


/*******************************************************************************
  Weak packet handlers

  Do not modify these, just create your own without the '__weak' attribute
*******************************************************************************/
/**
  *@brief Handler for receiving ping packets
  *@param ${proto.prefix}_ping ptr to incoming ping packet
  *@param ${proto.prefix}_ack ptr to repsonding ack
  *@return PACKET_HANDLED
  */
__attribute__((weak)) HandlerStatus_e ${proto.prefix}_Ping_handler(${proto.prefix}_packet_t* ${proto.prefix}_ping, ${proto.prefix}_packet_t* ${proto.prefix}_ack)
{
  /* Ack token has already been set as ping token with POLY_ACK_FLAG*/
  return PACKET_HANDLED;
}

/**
  *@brief Handler for receiving ack packets
  *@param ${proto.prefix}_ack ptr to ack
  *@return PACKET_HANDLED
  */
__attribute__((weak)) HandlerStatus_e ${proto.prefix}_Ack_handler(${proto.prefix}_packet_t* ${proto.prefix}_ack)
{
  return PACKET_HANDLED;
}

% for packet in proto.packets:
%if not packet.standard:
%if not packet.hasResponse:
/**
  *@brief Handler for receiving ${packet.name} packets
  *@param ${packet.name} incoming ${packet.name} packet
  *@return handling status
  */
__attribute__((weak)) HandlerStatus_e ${proto.prefix}_${packet.camel()}_handler(${proto.prefix}_packet_t* ${proto.prefix}_${packet.name})
%else:
/**
  *@brief Handler for receiving ${packet.name} packets
  *@param ${packet.name} incoming ${packet.name} packet
  *@param ${packet.response.name} ${packet.response.name} packet to respond with
  *@return handling status
  */
__attribute__((weak)) HandlerStatus_e ${proto.prefix}_${packet.camel()}_handler(${proto.prefix}_packet_t* ${proto.prefix}_${packet.name}, ${proto.prefix}_packet_t* ${proto.prefix}_${packet.response.name})
%endif
{
  /*  Get Required Fields in packet */
% for field in packet.fields:
%if field.isRequired:
  //${field.getParamType()} ${field.name} = ${proto.prefix}_get${field.camel()}(${proto.prefix}_${packet.name}); //${field.desc}
%endif
% endfor
%if packet.hasResponse:
  /*    Set required Fields in response  */
% for field in packet.response.fields:
%if field.isRequired:
  //${proto.prefix}_set${field.camel()}(${proto.prefix}_${packet.response.name}, value );  //${field.desc}
%endif
%endfor
%endif


  /* NOTE : This function should not be modified! If needed,  It should be overridden in the application code */

  return PACKET_NOT_HANDLED;
}

%endif
% endfor

/**
  *@brief catch-all handler for any packet not handled by its default handler
  *@param metaPacket ptr to ${proto.prefix}_packet_t containing packet
  *@return handling status
  */
__attribute__((weak)) HandlerStatus_e ${proto.prefix}_default_handler( ${proto.prefix}_packet_t * ${proto.prefix}_packet)
{

  /* NOTE : This function should not be modified, when the callback is needed,
          ${proto.prefix}_default_handler  should be implemented in the user file
  */

  return PACKET_NOT_HANDLED;
}
