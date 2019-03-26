
#ifdef UNIT_TESTING_ENABLED



extern "C"{

#include "../poly_field.c"
#include "../poly_packet.c"
#include "../poly_service.c"
#include "../poly_spool.c"
}
#include "../example/SampleService.c"

#include <gtest/gtest.h>


uint8_t txBuf[512]; //buffer used to fake serial writes

//Function to mock uart_tx on mcu
HandlerStatus_e mock_uart_send(uint8_t* data, int len)
{
  memcpy(txBuf,data,len);
  return PACKET_SENT;
}

/**
  *@brief catch-all handler for any packet not handled by its default handler
  *@param metaPacket ptr to tp_packet_t containing packet
  *@return handling status
  */
HandlerStatus_e tp_default_handler( tp_packet_t * tp_packet)
{

  /* NOTE : This function should not be modified, when the callback is needed,
          tp_default_handler  should be implemented in the user file
  */

  return PACKET_HANDLED;
}


TEST(PolyPacket_CTest, PackTest )
{
  tp_service_init(1); // initialize with 1 interface

  tp_packet_t msg;
  tp_ppacket_init(&msg, )



  tp_service_teardown();
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
