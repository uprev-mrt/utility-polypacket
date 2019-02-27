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

PolyTest::PolyTest()
:PolyPacket(PP_TestPacket)
{
  if(packet != NULL)
  {
    memcpy(&mPacket, &packet, sizeof(poly_packet_t))
  }
  //bind all fields
  getField(PF_TestString)->mData = (uint8_t*) &mTestString;
  getField(PF_TestInt)->mData = (uint8_t*) &mTestInt;
  getField(PF_TestUint16)->mData = (uint8_t*) &mTestUint16;

  mPacket->mBound = true;
}

//getters
string PolyTest::TestString() const { return string(mTestString);}
int PolyTest::TestInt() const {return mTestInt;}
uint16_t PolyTest::TestUint16() const {return mTestUint16;}

bool PolyTest::hasTestString() const {return hasField(PF_TestString);}
bool PolyTest::hasTestInt() const {return hasField(PF_TestInt);}
bool PolyTest::hasTestUint16() const {return hasField(PF_TestUint16);}


//setters
void PolyTest::TestString(string val)
{
  hasField(PF_TestString, true);
  memcpy(mTestString, val.c_str(), min((int)val.length(), 16));
}

void PolyTest::TestInt(int val)
{
  hasField(PF_TestInt, true);
  mTestInt = val;
}

void PolyTest::TestUint16(uint16_t val)
{
  hasField(PF_TestUint16, true);
  mTestUint16 = val;
}

void PolyTest::hasTestString(bool val) {return hasField(PF_TestString, val);}
void PolyTest::hasTestInt(bool val) {return hasField(PF_TestInt, val);}
void PolyTest::hasTestUint16(bool val) {return hasField(PF_TestUint16, val);}
