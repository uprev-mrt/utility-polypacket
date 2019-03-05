
#ifdef UNIT_TESTING_ENABLED



#include "poly_packet.c"
#include "poly_field.c"
#include "PolyPacket.cpp"
#include "example/SampleProtocol.cpp"

#include <gtest/gtest.h>

using namespace Utilities::PolyPacket;



uint8_t buffer[1024];

//Test ints
TEST(PolyFieldTest, Json Test )
{
  SampleProtocol_protocol_init();

  SetdataPacket msg;

  msg.Src(0xABCD);
  msg.Dst(0xCDEF);
  msg.Sensora(32500);
  msg.Sensorb(898989);
  msg.Sensorname("This is my test string");

  int len = msg.pack(buffer);


  ASSERT_EQ(msg.toJSON() , "{\"src\" : \"xCDAB\" , \"dst\" : \"xEFCD\" , \"sensorA\" : 32500 , \"sensorB\" : 898989 , \"sensorName\" : \"This is my test string\"}");

  for(int i; i < len; i++)
  {

  }

}


//Test ints
TEST(PolyFieldTest, packetTest)
{


}




#endif
