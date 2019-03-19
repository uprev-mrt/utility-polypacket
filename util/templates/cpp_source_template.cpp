/**
  *@file ${proto.fileName}.cpp
  *@brief generated protocol source code
  *@author make_protocol.py
  *@date 03/18/19
  */

/***********************************************************
        THIS FILE IS AUTOGENERATED. DO NOT MODIFY
***********************************************************/

#include "${proto.fileName}.h"
#include "Utilities/PolyPacket/poly_service.h"


//Define packet IDs
% for packet in proto.packets:
#define ${} ${packet.globalName}_ID ${packet.packetId}
% endfor


//Global descriptors
% for packet in proto.packets:
poly_packet_desc_t* ${packet.globalName}
% endfor

% for field in proto.fields:
poly_field_desc_t* ${field.globalName}
% endfor

poly_service_t* ${proto.service()};

/*******************************************************************************
  Service Process
*******************************************************************************/
/**
  *@brief attempts to process data in buffers and parse out packets
  */
void ${proto.prefix}_service_process()
{
  static ${proto.prefix}_packet_t metaPacket;
  poly_packet_t* newPacket;

  uint8_t handlingStatus = PACKET_UNHANDLED;

  if(poly_service_try_parse(${proto.service()}) == PACKET_VALID)
  {
    ${proto.prefix}_packet_init(&metaPacket, newPacket);

    //Dispatch packet
    switch(metaPacket->mTypeId)
    {
  % for packet in proto.packets:
      case ${packet.globalName}_ID:
        handlingStatus = ${proto.prefix}_${packet.name.lower()}_handler(metaPacket.mPayload.${packet.name.lower()});
        break;
  % endfor
      default:
        //we should never get here
        asser(false);
        break;
    }

    //If the packet was not handled, throw it to the default handler
    if(handlingStatus == PACKET_UNHANDLED)
      handlingStatus = ${proto.prefix}_default)handler(&metaPacket);

    ${proto.prefix}_packet_teardown(&metaPacket);
  }

}

/*******************************************************************************
  Service initializer
*******************************************************************************/

/**
  *@brief initializes ${proto.prefix}_protocol
  *@param interfaceCount number of interfaces to create
  */
void ${proto.prefix}_service_init(int interfaceCount)
{

  ${proto.service()} = new_poly_service(${len(proto.packets)}, interfaceCount);

  //Build Packet Descriptors
% for packet in proto.packets:
  ${packet.globalName} = new_poly_packet_desc("${packet.name}", ${len(packet.fields)});
% endfor

  //Build Field Descriptors
% for field in proto.fields:
  ${field.globalName} = new_poly_field_desc("${packet.name}", TYPE_${field.type.upper()}, ${field.arrayLen}, ${field.format.upper()});
% endfor

% for packet in proto.packets:
  //Settomg Field Descriptors for ${packet.name}
  % for field in packet.fields:
  poly_packet_desc_add_field(${packet.globalName} , ${field.globalName} , ${str(field.isRequired).lower()} );
  % endfor
% endfor

  //Register packet descriptors with the service
% for packet in proto.packets:
  poly_service_register_desc(${proto.service()}, ${packet.globalName});
% endfor

}

/*******************************************************************************
  Meta packet
*******************************************************************************/
void ${proto.prefix}_packet_init(${proto.prefix}_packet_t* metaPacket, poly_packet_t* packet)
{
  //set typeId
  metaPacket->mTypeId = packet->mDesc->mTypeId;

  //create a new unallocated packet
  poly_packet_t* newPacket = new_poly_packet(desc, false);


  switch(metaPacket->mTypeId)
  {
% for packet in proto.packets:
    case ${packet.globalName}_ID:
    ${proto.prefix}_${packet.name.lower()}_bind(metaPacket->mPayload.${packet.name.lower()}, newPacket);
    break;
% endfor
  }
}

void ${proto.prefix}_packet_teardown(${proto.prefix}_packet_t* metaPacket)
{
  poly_packet_t* packet;
  switch(metaPacket->mTypeId)
  {
% for packet in proto.packets:
    case ${packet.globalName}_ID:
    packet = metaPacket->mPayload.${packet.name.lower()}->mPacket;
    free(metaPacket->mPayload.${packet.name.lower()});
    break;
% endfor
  }

  poly_packet_destroy(packet);
}


//Meta packet setters
% for field in proto.fields:
void ${proto.prefix}_set${field.name.capitalize()}(${proto.prefix}_packet_t* packet, ${field.getParamType()} val)
{
  poly_field_t* field = poly_packet_get_field(packet->mPacket, packet->mPacket->mDesc);
%if field.isArray:
  poly_field_set(field,( const uint8_t*) val);
% else:
  poly_field_set(field,( const uint8_t*) &val);
% endif
}

% endfor

//Meta packet getters
% for field in proto.fields:
${field.getParamType()} ${proto.prefix}_get${field.name.capitalize()}(${proto.prefix}_packet_t* packet)
{
  ${field.getParamType()} val;
%if field.isArray:
  val = (${field.getParamType()})poly_field_get(field);
% else:
  poly_field_get(field,(uint8_t*) &val);
% endif
  return val;
}

% endfor


/*******************************************************************************
  Packet Binding
*******************************************************************************/
% for packet in proto.packets:
/**
  *@brief Binds struct to poly_service_t
  *@param ${packet.name.lower()} ptr to ${packet.structName} to be bound
  *@param packet packet to bind to
  */
void ${packet.name.lower()}_bind(${packet.structName}* ${packet.name.lower()}, poly_packet_t* packet)
{
  ${packet.name.lower()}->mPacket = packet;

% for field in packet.fields:
  poly_field_bind( poly_packet_get_field(packet, ${field.globalName}), (uint8_t*) &${packet.name.lower()}->${field.memberName});
% endfor

}

% endfor

/*******************************************************************************
  Weak packet handlers

  Do not modify these, just create your own without the '__weak' attribute
*******************************************************************************/
% for packet in proto.packets:
__weak uint8_t ${proto.prefix}_${packet.name.lower()}_handler(${packet.structName} * packet)
{
  return PACKET_UNHANDLED;
}
% endfor
