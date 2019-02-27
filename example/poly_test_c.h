#pragma once
#include "../poly_field.h"
#include "../poly_packet.h"

extern poly_packet_desc_t* PP_TestPacket;
extern poly_field_desc_t* PF_TestString;
extern poly_field_desc_t* PF_TestInt;
extern poly_field_desc_t* PF_TestUint16;

typedef struct{
  char testString[16];
  int testInt;
  uint16_t testUint16;
  poly_packet_t* mPacket;
} test_struct_t;

//initializes protocol
void poly_test_init();

//creates new object
test_struct_t* new_test_struct();


//destroys object
void destroy_test_struct(test_struct_t* t);
