#include "poly_test.h"

poly_packet_desc_t* PP_TestPacket;
poly_field_desc_t* PF_TestString;
poly_field_desc_t* PF_TestInt;
poly_field_desc_t* PF_TestUint16;


void poly_test_init()
{
  PP_TestPacket = new_poly_packet_desc("PacketTest", 3);

  PF_TestString = new_poly_field_desc("testString", TYPE_STRING, 16,FORMAT_ASCII);
  PF_TestInt = new_poly_field_desc("testint", TYPE_INT, 1,FORMAT_DEC);
  PF_TestUint16 = new_poly_field_desc("testUint16", TYPE_UINT16, 1,FORMAT_HEX);

  poly_packet_desc_add_field(PP_TestPacket, PF_TestString, true);
  poly_packet_desc_add_field(PP_TestPacket, PF_TestInt, true);
  poly_packet_desc_add_field(PP_TestPacket, PF_TestUint16, true);

}

test_struct_t* new_test_struct()
{
  test_struct_t* newStruct = (test_struct_t*) malloc(sizeof(test_struct_t));
  newStruct->mPacket = new_poly_packet(PP_TestPacket);

  poly_packet_get_field(newStruct->mPacket, PF_TestString)->mData = (uint8_t*) &newStruct->testString;
  poly_packet_get_field(newStruct->mPacket, PF_TestInt)->mData = (uint8_t*) &newStruct->testInt;
  poly_packet_get_field(newStruct->mPacket, PF_TestUint16)->mData = (uint8_t*) &newStruct->testUint16;

  newStruct->mPacket->mBound = true;

  return newStruct;
}

void destroy_test_struct(test_struct_t* t)
{

  poly_packet_destroy(t->mPacket);

  free(t);
}
