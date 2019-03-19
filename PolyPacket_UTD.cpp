//
// #ifdef UNIT_TESTING_ENABLED
//
//
//
// #include "poly_packet.c"
// #include "poly_field.c"
// #include "poly_service.c"
// #include "example/SampleProtocol.c"
//
// #include <gtest/gtest.h>
//
// using namespace Utilities::PolyPacket;
//
//
//
// uint8_t buffer[1024];
// int len;
// uint8_t compBuffer[] = {0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x5F, 0x0E, 0xCD, 0xAB, 0xEF, 0xCD, 0xF4, 0x7E, 0xAD, 0xB7, 0x0D, 0x00, 0x17, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x6D, 0};
// SetdataPacket msg;
// SetdataPacket msg2;
//
//
// TEST(PolyPacketTest, JsonTest )
// {
//
// }
//
//
//
// //
// // //Test ints
// // TEST(PolyPacketTest, JsonTest )
// // {
// //
// //
// //
// //   msg.Src(0xABCD);
// //   msg.Dst(0xCDEF);
// //   msg.Sensora(32500);
// //   msg.Sensorb(898989);
// //   msg.Sensorname("This is my test string");
// //
// //   //validate json
// //   ASSERT_EQ(msg.toJSON(false) , "{\"src\" : \"xCDAB\" , \"dst\" : \"xEFCD\" , \"sensorA\" : 32500 , \"sensorB\" : 898989 , \"sensorName\" : \"This is my test string\"}");
// //
// // }
// //
// //
// // //Test ints
// // TEST(PolyPacketTest, PackTest)
// // {
// //   len = msg.pack(buffer);
// //
// //   //validate length
// //   ASSERT_EQ(len , 43 );
// //
// //   //verify mCheckSum
// //   ASSERT_EQ( msg.Checksum(), 3903 );
// //
// // }
// //
// // //Test ints
// // TEST(PolyPacketTest, ParseTest)
// // {
// //   msg2.parse(buffer, len);
// //
// //   //validate json
// //   ASSERT_EQ(msg2.toJSON(false) , "{\"src\" : \"xCDAB\" , \"dst\" : \"xEFCD\" , \"sensorA\" : 32500 , \"sensorB\" : 898989 , \"sensorName\" : \"This is my test string\"}");
// //
// //
// // }
//
//
//
//
// #endif
