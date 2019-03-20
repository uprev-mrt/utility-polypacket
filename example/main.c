#include "SampleProtocol.h"
#include <stdio.h>

uint8_t buffer[1024];
char printBuf[1024];
int len;

HandlerStatus_e sp_setdata_handler(setdata_packet_t * packet)
{
  /* NOTE : This function should not be modified, when the callback is needed,
          sp_setdata_handler  should be implemented in the user file
  */
  sp_print_json(packet, printBuf);

  printf("handled! = %s\n", printBuf);
  return PACKET_UNHANDLED;
}

void sendUart(uint8_t* data, int len)
{
  printf("message sent!\n");
}

int main()
{
  printf("\nBuilding Test packet C++\n\n*******************************\n\n");
  sp_service_init(2);

  sp_packet_t* msg = new_sp_packet(SP_SETDATA_PACKET);
  sp_packet_t* msgb = new_sp_packet(SP_SETDATA_PACKET);

  sp_setSrc(msg,0xABCD );
  sp_setDst(msg,0xCDEF);
  sp_setSensora(msg,32500);
  sp_setSensorb(msg,898989);
  sp_setSensorname(msg, "This is my test string");

  sp_service_register_tx(0, &sendUart);

  len = sp_pack(msg, buffer);


  int tp = sp_parse(msgb, buffer, len);

  sp_print_json(msgb, printBuf);

  printf("Feeding = %s\n", printBuf);
  sp_service_feed(0,buffer,len);

  sp_send(0,msg);

  sp_service_process();



  return 0;
}
