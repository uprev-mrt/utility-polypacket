#include "SampleProtocol.h"
#include <stdio.h>

uint8_t buffer[1024];
char printBuf[1024];


int main()
{
  printf("\nBuilding Test packet C++\n\n*******************************\n\n");

  SetdataPacket setDataPacket;

  printf("Test point 2\n" );
  setDataPacket.Src(0xABCD);
  setDataPacket.Dst(0xCDEF);
  setDataPacket.Sensora(32500);
  setDataPacket.Sensorb(898989);
  setDataPacket.Sensorname("This is my test string");

  int len = setDataPacket.pack(buffer);

  printf("[Packed] :");
  for(int i=0; i < len; i++)
  {
    printf(" %02X", buffer[i]);
  }


  printf("\n\n[JSON] : %s\n", setDataPacket.toJSON(false).c_str());

  return 0;
}
