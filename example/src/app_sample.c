/**
  *@file app_sample.c
  *@brief generated protocol source code
  *@author make_protocol.py
  *@date 03/29/19
  */

/***********************************************************
        Application Layer
***********************************************************/

#include "app_sample.h"
#include "linux_uart.h"

static uint8_t iface0_rx_buf[512];
int fd;
static char printBuf[512];

#define SP_PRINT(packet) fp_print_json((packet),printBuf); printf("%s",printBuf)

static inline HandlerStatus_e iface0_write(uint8_t* data, int len)
{
  /* Place code for writing bytes on interface 0 here */
  uart_write(fd,data,len);

  return PACKET_SENT;
}


static inline void iface0_read()
{

  /* Place code for reading bytes from interface 0 here */
  int len = uart_read(fd,iface0_rx_buf, 32);

  sp_service_feed(0,iface0_rx_buf, len);
}

/*******************************************************************************
  App Init/end
*******************************************************************************/
void app_sample_init(const char* port, int baud)
{
  /* initialize peripheral for iface_0 */
  if(uart_open(&fd,port, baud))
    printf("successfully opened port: %s\n",port);
  else
    printf("Could not open port: %s",port);

  //initialize service
  sp_service_init(1);

  sp_service_register_tx(0, iface0_write);

}

void app_sample_end()
{
  uart_close(fd);
}

/*******************************************************************************
  App Process
*******************************************************************************/

void app_sample_process()
{
  /* read in new data from iface 0*/
  iface0_read();

  /* process the actual service */
  sp_service_process();

}


/*******************************************************************************
  Packet handlers
*******************************************************************************/
/**
  *@brief Handler for receiving SendCmd packets
  *@param SendCmd incoming SendCmd packet
  *@return handling status
  */
HandlerStatus_e sp_SendCmd_handler(sp_packet_t* sp_SendCmd)
{
  /*  Get Required Fields in packet */
  uint8_t cmd = sp_getCmd(sp_SendCmd);    //
  switch(cmd)
  {
    case SP_CMD_LED_ON:
      // turns on led
      break;
    case SP_CMD_LED_OFF:
      // turns off led
      break;
    case SP_CMD_RESET:
      // resets the device
      break;
  }


  return PACKET_HANDLED;
}

/**
  *@brief Handler for receiving GetData packets
  *@param GetData incoming GetData packet
  *@param Data Data packet to respond with
  *@return handling status
  */
HandlerStatus_e sp_GetData_handler(sp_packet_t* sp_GetData, sp_packet_t* sp_Data)
{
  /*  Get Required Fields in packet */
  /*    Set required Fields in response  */


  return PACKET_HANDLED;
}

/**
  *@brief Handler for receiving Data packets
  *@param Data incoming Data packet
  *@return handling status
  */
HandlerStatus_e sp_Data_handler(sp_packet_t* sp_Data)
{
  /*  Get Required Fields in packet */


  return PACKET_HANDLED;
}


/**
  *@brief catch-all handler for any packet not handled by its default handler
  *@param metaPacket ptr to sp_packet_t containing packet
  *@return handling status
  */
HandlerStatus_e sp_default_handler( sp_packet_t * sp_packet)
{


  return PACKET_HANDLED;
}
