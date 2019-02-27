#include "SampleProtocol_proto.h"
#include <stdio.h>

uint8_t buffer[1024];
char printBuf[1024];


int main()
{
  printf("test\n");
  SampleProtocol_protocol_init();


  SetdataPacket setDataPacket;


  printf("%s\n", setDataPacket.toJSON().c_str());

  return 0;
}
