
#ifdef UNIT_TESTING_ENABLED

#include "PolyField.cpp"
#include "PolyPacket.cpp"
#include "TestPacket.h"
#include <gtest/gtest.h>

using namespace Utilities;


//Test ints
TEST(PolyFieldTest, test1)
{
  PolyFieldDesc desc("test", sizeof(char), 16);
  PolyField field(desc);

  field.setString("This is a test!");

  string str = field.getString();

  ASSERT_EQ(str,"This is a test!");

}


//Test ints
TEST(PolyFieldTest, packetTest)
{
  PolyFieldDesc f1("field1", sizeof(char), 16);
  PolyFieldDesc f2("field2", sizeof(int), 1);
  PolyFieldDesc f3("field3", sizeof(uint8_t), 1);
  PolyFieldDesc f4("field4", sizeof(double), 1);
  PolyFieldDesc f5("field5", sizeof(int), 2);

  PolyPacketDesc p("testPacket");
  p.addField(f1, false);
  p.addField(f2, false);
  p.addField(f3, false);
  p.addField(f4, false);
  p.addField(f5, false);

//  TestPacket tp(p);

  //tp.F1("Test string");
  //  tp.F2(4356);
//  tp.F3(43);

  //ASSERT_EQ(tp.F1(),"Test string");
  //ASSERT_EQ(tp.F2(),4357);
  //ASSERT_EQ(tp.F3(),43);

}




#endif
