
#ifdef UNIT_TESTING_ENABLED



#include "poly_packet.c"
#include "poly_field.c"
#include "PolyPacket.cpp"
#include "example/SampleProtocol.cpp"

#include <gtest/gtest.h>

using namespace Utilities::PolyPacket;


//Test ints
TEST(PolyFieldTest, test1)
{
  SampleProtocol_protocol_init();

  SetdataPacket msg;

  msg.Src(197);
  msg.Dst(208);
  msg.Sensorname("testName");


  ASSERT_EQ(msg.toJSON() , "{\"src\" : \"xC500\" , \"dst\" : \"xD000\" , \"sensorName\" : \"testName\"}");


}


//Test ints
TEST(PolyFieldTest, packetTest)
{


}




#endif
