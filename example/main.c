#include "SampleService.h"
#include <stdio.h>

uint8_t buffer[1024];
char printBuf[1024];
int len;

#define UART 0


//Function to mock uart_tx on mcu
HandlerStatus_e mock_uart_send(uint8_t* data, int len)
{

  return PACKET_HANDLED;
}

//Function mocking uart_rx on mcu
void mock_uart_receive(uint8_t* data, int len)
{
  // printf("message Recieved!\n");
  // //when we receive data, we 'Feed' it to the service for parsing
  // sp_service_feed(UART,data,len);
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

  // //
  // sp_packet_t msg;
  // sp_packet_build(&msg, SP_PACKET_SENDCMD);
  // sp_setCmd(&msg,33);
  //
  // int len = sp_pack(&msg, buffer);


  //sp_service_feed(0,ackData, 8);
  //sp_sendGetData(0);

  for(int i=0; i < 2048; i++)
  {
    //sp_sendSendCmd(0,0);
    sp_sendData(0, 32500,898989, "This is my test string");
    sp_service_process();
    sp_service_feed(0,buffer, len);
    sp_service_process();
  }




  //poly_packet_t msg;
  //poly_packet_build(&msg, SP_PACKET_DATA, true);

  //poly_packet_clean(&msg);

  // while(1)
  // {
  //     sp_sendSendCmd(0,0);
  //     //sp_service_process();
  //     sleep(0.1);
  // }

  sp_service_teardown();

  return 0;
}
