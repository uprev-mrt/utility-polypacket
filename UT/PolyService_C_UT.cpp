
#ifdef UNIT_TESTING_ENABLED

//#define POLY_PACKET_DEBUG_LVL 3


extern "C"{

#include "../poly_field.c"
#include "../poly_packet.c"
#include "../poly_service.c"
#include "../poly_spool.c"

#include "testService.c"
}


#include <gtest/gtest.h>


uint8_t txBuf[512]; //buffer used to fake serial writes
char printBuf[128];

//Function to mock uart_tx on mcu
HandlerStatus_e mock_uart_send(uint8_t* data, int len)
{
  tp_service_feed(0,data,len);
  return PACKET_HANDLED;
}



TEST(PolyPacket_CTest, PackTest )
{
  tp_service_init(1); // initialize with 1 interface

  tp_service_register_tx(0, &mock_uart_send);

  int len;
  tp_packet_t msg;
  tp_packet_build(&msg,TP_PACKET_SENDCMD );
  tp_setCmd(&msg, 0x43);

  tp_send(0,&msg);

//  len = poly_packet_pack(&msg.mPacket, txBuf);
  //tp_service_feed(0,txBuf, len);

  tp_service_process();
  tp_service_process();

  tp_service_teardown();
}



#endif
