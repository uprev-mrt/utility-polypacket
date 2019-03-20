
#ifdef UNIT_TESTING_ENABLED



extern "C"{

#include "poly_field.c"
#include "poly_packet.c"
#include "poly_service.c"
}
#include "example/SampleProtocol.c"

#include <gtest/gtest.h>



uint8_t buffer[1024];
int len;

uint8_t compBuffer[] = {0x01, 0x00, 0x2C, 0x00, 0x00, 0x00, 0xBB, 0x10, 0xE0, 0xCD, 0xAB, 0xEF, 0xCD, 0xF4, 0x7E, 0xAD, 0xB7, 0x0D, 0x00, 0x20, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x6D, 0x79, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x68, 0x6F, 0x6D};

TEST(PolyPacket_CTest, PackTest )
{
  ePacketStatus status;
  sp_service_init(1);
  sp_packet_t* msg = new_sp_packet(PP_SETDATA_PACKET);
  sp_packet_t* msgb = new_sp_packet(PP_SETDATA_PACKET);

  sp_setSrc(msg,0xABCD );
  sp_setDst(msg,0xCDEF);
  sp_setSensora(msg,32500);
  sp_setSensorb(msg,898989);
  sp_setSensorname(msg, "This is my test string");

  len = sp_pack(msg, buffer);

  ASSERT_EQ(len,52);

  status = sp_parse(msgb, buffer, len);


}

//
// //Test ints
// TEST(PolyPacketTest, JsonTest )
// {
//
//
//
//   msg.Src(0xABCD);
//   msg.Dst(0xCDEF);
//   msg.Sensora(32500);
//   msg.Sensorb(898989);
//   msg.Sensorname("This is my test string");
//
//   //validate json
//   ASSERT_EQ(msg.toJSON(false) , "{\"src\" : \"xCDAB\" , \"dst\" : \"xEFCD\" , \"sensorA\" : 32500 , \"sensorB\" : 898989 , \"sensorName\" : \"This is my test string\"}");
//
// }
//
//
// //Test ints
// TEST(PolyPacketTest, PackTest)
// {
//   len = msg.pack(buffer);
//
//   //validate length
//   ASSERT_EQ(len , 43 );
//
//   //verify mCheckSum
//   ASSERT_EQ( msg.Checksum(), 3903 );
//
// }
//
// //Test ints
// TEST(PolyPacketTest, ParseTest)
// {
//   msg2.parse(buffer, len);
//
//   //validate json
//   ASSERT_EQ(msg2.toJSON(false) , "{\"src\" : \"xCDAB\" , \"dst\" : \"xEFCD\" , \"sensorA\" : 32500 , \"sensorB\" : 898989 , \"sensorName\" : \"This is my test string\"}");
//
//
// }




#endif
