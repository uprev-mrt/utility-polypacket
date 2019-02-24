#include "poly_test.h"
#include <stdio.h>

uint8_t buffer[1024];

int main()
{
  printf("test\n");
  poly_test_init();
  test_struct_t* objA = new_test_struct();
  test_struct_t* objB = new_test_struct();

  //char* test = "This is a test";

  //memcpy(objA->testString, test, strlen(test));

  objA->testInt = 4;
  objA->testUint16 = 45;

  int len = poly_packet_pack(objA->mPacket, buffer);
  poly_packet_parse(objB->mPacket, objB->mPacket->mDesc, buffer, len);


  return 0;
}
