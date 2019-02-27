#pragma once
#include "../poly_field.h"
#include "../poly_packet.h"
#include "../PolyPacket.h"

extern poly_packet_desc_t* PP_TestPacket;
extern poly_field_desc_t* PF_TestString;
extern poly_field_desc_t* PF_TestInt;
extern poly_field_desc_t* PF_TestUint16;

using namespace Utilities::PolyPacket;

class PolyTest : public PolyPacket
{
public:
  PolyTest();

  //getters
  string TestString() const;
  int TestInt() const;
  uint16_t TestUint16() const;
  bool hasTestString() const;
  bool hasTestInt() const;
  bool hasTestUint16() const;


  //setters
  void TestString(string val);
  void TestInt(int val);
  void TestUint16(uint16_t val);

  void hasTestString(bool val);
  void hasTestInt(bool val);
  void hasTestUint16(bool val);

private:
  char mTestString[16];
  int mTestInt;
  uint16_t mTestUint16;
};



//initializes protocol
void poly_test_init();
