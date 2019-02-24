#include "poly_test.h"
#include <stdio.h>

uint8_t buffer[1024];
char printBuf[1024];

int main()
{
  printf("test\n");
  poly_test_init();
  test_struct_t* objA = new_test_struct();
  test_struct_t* objB = new_test_struct();

  char* test = "This is a test";

  memcpy(objA->testString, test, strlen(test));

  objA->mPacket->mFields[0].mPresent = true;
  objA->testInt = 4;
  objA->mPacket->mFields[1].mPresent = true;
  objA->testUint16 = 45;
  objA->mPacket->mFields[2].mPresent = true;

  int len = poly_packet_pack(objA->mPacket, buffer);
  poly_packet_parse(objB->mPacket, objB->mPacket->mDesc, buffer, len);


  poly_packet_print_json(objA->mPacket, printBuf, true);
  printf("%s\n", printBuf );

  return 0;
}
