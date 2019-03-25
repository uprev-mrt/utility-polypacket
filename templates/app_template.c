/**
  *@file app_${proto.name.lower()}.c
  *@brief generated protocol source code
  *@author make_protocol.py
  *@date ${proto.genTime}
  */

/***********************************************************
        Application Layer
***********************************************************/

#include "app_${proto.name.lower()}.h"
%if proto.genUtility:
#include "linux_uart.h"
%endif

static uint8_t iface0_rx_buf[512];
%if proto.genUtility:
int fd;
static char printBuf[512];

#define ${proto.prefix.upper()}_PRINT(packet) fp_print_json((packet),printBuf); printf("%s",printBuf)
%endif

static inline HandlerStatus_e iface0_write(uint8_t* data, int len)
{
  /* Place code for writing bytes on interface 0 here */
%if proto.genUtility:
  uart_write(fd,data,len);
%else:
  //TODO write to interface (i.e. uart,spi, etc)
%endif

  return PACKET_SENT;
}


static inline void iface0_read()
{

  /* Place code for reading bytes from interface 0 here */
%if proto.genUtility:
  int len = uart_read(fd,iface0_rx_buf, 32);
%else:
  //TODO read bytes from interface to iface0_rx_buf
  int len;
%endif

  ${proto.prefix}_service_feed(0,iface0_rx_buf, len);
}

/*******************************************************************************
  App Init/end
*******************************************************************************/
%if proto.genUtility:
void app_${proto.name.lower()}_init(const char* port, int baud)
{
  /* initialize peripheral for iface_0 */
  if(uart_open(&fd,port, baud))
    printf("successfully opened port: %s\n",port);
  else
    printf("Could not open port: %s",port);
%else:
void app_${proto.name.lower()}_init()
{
  /* initialize peripheral for iface_0 */
  //TODO initialize peripheral for iface_0
%endif

  //initialize service
  ${proto.prefix}_service_init(1);

  ${proto.prefix}_service_register_tx(0, iface0_write);

}

void app_${proto.name.lower()}_end()
{
%if proto.genUtility:
  uart_close(fd);
%endif
}

/*******************************************************************************
  App Process
*******************************************************************************/

void app_${proto.name.lower()}_process()
{
  /* read in new data from iface 0*/
  iface0_read();

  /* process the actual service */
  ${proto.prefix}_service_process();

}


/*******************************************************************************
  Packet handlers
*******************************************************************************/
% for packet in proto.packets:
% if not packet.standard:
%if not packet.hasResponse:
/**
  *@brief Handler for receiving ${packet.name} packets
  *@param ${packet.name} incoming ${packet.name} packet
  *@return handling status
  */
HandlerStatus_e ${proto.prefix}_${packet.camel()}_handler(${proto.prefix}_packet_t* ${proto.prefix}_${packet.name})
%else:
/**
  *@brief Handler for receiving ${packet.name} packets
  *@param ${packet.name} incoming ${packet.name} packet
  *@param ${packet.response.name} ${packet.response.name} packet to respond with
  *@return handling status
  */
HandlerStatus_e ${proto.prefix}_${packet.camel()}_handler(${proto.prefix}_packet_t* ${proto.prefix}_${packet.name}, ${proto.prefix}_packet_t* ${proto.prefix}_${packet.response.name})
%endif
{
  /*  Get Required Fields in packet */
% for field in packet.fields:
%if field.isRequired:
  ${field.getParamType()} ${field.name} = ${proto.prefix}_get${field.camel()}(${proto.prefix}_${packet.name});
%endif
% endfor
% for field in packet.fields:
%if field.isRequired and field.isEnum:
  switch(${field.name})
  {
  % for val in field.vals:
    case ${proto.prefix.upper()+"_"+field.name.upper() + "_" + val.name.upper()}:
      // ${val.desc}
      break;
  % endfor
  }
%endif
% endfor
%if packet.hasResponse:
  /*    Set required Fields in response  */
% for field in packet.response.fields:
%if field.isRequired:
  //${proto.prefix}_set${field.camel()}(${proto.prefix}_${packet.response.name}, value );                   //Set ${field.name} value
%endif
%endfor
%endif


  return PACKET_HANDLED;
}

% endif
% endfor

/**
  *@brief catch-all handler for any packet not handled by its default handler
  *@param metaPacket ptr to ${proto.prefix}_packet_t containing packet
  *@return handling status
  */
HandlerStatus_e ${proto.prefix}_default_handler( ${proto.prefix}_packet_t * ${proto.prefix}_packet)
{


  return PACKET_HANDLED;
}
