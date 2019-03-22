#include "SampleProtocol.h"
#include <stdio.h>

uint8_t buffer[1024];
char printBuf[1024];
int len;

#define UART 0

/*  Handler function for 'SetData' messages
 *  This is created by the service and declared '__weak__' so we just have to provide our own function to override
 */
HandlerStatus_e sp_setdata_handler(sp_packet_t* SetData, sp_packet_t* RespData)
{
  sp_setSrc(RespData,0xABCD );
  sp_setDst(RespData,0xCDEF);
  sp_setSensorA(RespData,32500);
  sp_setSensorB(RespData,898989);
  //convert the message to a json string
  sp_print_json(SetData, printBuf);
  printf("handled! = %s\n", printBuf);

  return PACKET_HANDLED;
}

//Function to mock uart_tx on mcu
HandlerStatus_e mock_uart_send(uint8_t* data, int len)
{

  printf("message sent: ");
  for(int i=0 ; i < len; i++)
  {
    printf(" %02X", data[i]);
  }
  printf("\n" );
  return PACKET_HANDLED;
}

//Function mocking uart_rx on mcu
void mock_uart_receive(uint8_t* data, int len)
{
  printf("message Recieved!\n");
  //when we receive data, we 'Feed' it to the service for parsing
  sp_service_feed(UART,data,len);
}


int main()
{
  /*initialize service with 1 interface
   * In this case we are just mocking on UART interface
   */
  sp_service_init(1);

  /*register a TX function for the interface
   */
  sp_service_register_tx(UART, &mock_uart_send);


  //create a new message
  sp_packet_t* msg = new_sp_packet(SP_PACKET_SETDATA);

  //Set the fields in the message
  sp_setSrc(msg,0xABCD );
  sp_setDst(msg,0xCDEF);
  sp_setSensorA(msg,32500);
  sp_setSensorB(msg,898989);
  sp_setSensorName(msg, "This is my test string");

  //send data over the UART interface
  //sp_send(UART,msg);

  int len = sp_pack(msg,buffer);

  for(int i=0 ; i < len; i++)
  {
    printf(" %02X", buffer[i]);
  }
  printf("\n" );

  sp_send(UART,msg);


  sp_service_feed(UART,buffer,len);

  sp_clean(msg);

  sp_service_process();

  while(1)
  {
    sp_service_process();
  }

  return 0;
}
