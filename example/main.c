#include "SampleProtocol.h"
#include <stdio.h>

uint8_t buffer[1024];
char printBuf[1024];
int len;

int main()
{
  printf("\nBuilding Test packet C++\n\n*******************************\n\n");

  sp_packet_t* msg = new_sp_packet(SETDATA_P_DESC);

  sp_setSrc(msg,0xABCD );
  sp_setDst(msg,0xCDEF);
  sp_setSensora(msg,32500);
  sp_setSensorb(msg,898989);
  sp_setSensorname(msg, "This is my test string");

  len = sp_pack(msg, buffer);

  return 0;
}
