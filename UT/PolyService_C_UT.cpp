
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

extern poly_service_t TP_SERVICE;

int packet_count =0;
const poly_packet_desc_t* lastRxDesc;

uint8_t txBuf[512]; //buffer used to fake serial writes
char printBuf[128];
bool corrupt = false;

//scratch buffer for building encoded packets
uint8_t encoded[128];


//Function to mock uart_tx on mcu
HandlerStatus_e mock_uart_send(uint8_t* data, int len)
{

  tp_service_feed(0,data,len);

  return PACKET_HANDLED;
}

/**
  *@brief catch-all handler for any packet not handled by its default handler
  *@param metaPacket ptr to tp_packet_t containing packet
  *@return handling status
  */
HandlerStatus_e tp_default_handler( tp_packet_t * tp_packet)
{

  lastRxDesc = tp_packet->mPacket.mDesc;
  packet_count++;
  return PACKET_HANDLED;
}


void feed_packet(int interface, tp_packet_t* packet)
{
  int len = poly_packet_pack_encoded(&packet->mPacket,encoded);

  tp_service_feed(interface,encoded,len);
}

int getFrameCount(int i)
{
  return TP_SERVICE.mInterfaces[i].mBytefifo.mFrameCount;
}

TEST(PolyService, FeedTest )
{
  tp_service_init(1); // initialize with 1 interface

  int frames;
  tp_packet_t msg;
  tp_packet_build(&msg,TP_PACKET_SENDCMD );

  tp_setCmd(&msg, 0x43);

  feed_packet(0,&msg);

  frames = getFrameCount(0);
  ASSERT_EQ(frames,1);

  poly_packet_clean(&msg.mPacket);
  tp_service_teardown();
}

TEST(PolyService, JsonFeedTest )
{
  tp_service_init(1); // initialize with 1 interface

  int frames;
  tp_packet_t msg;
  tp_packet_build(&msg,TP_PACKET_SENDCMD );

  tp_setCmd(&msg, 0x43);

  tp_print_json(&msg, printBuf);

  tp_service_feed_json(0, printBuf,strlen(printBuf));

  frames = getFrameCount(0);
  ASSERT_EQ(frames,1);

  poly_packet_clean(&msg.mPacket);
  tp_service_teardown();
}

/**
  *@brief feed 3 messages to incoming buffer and verufy they process
  */
TEST(PolyService, JSON_byte_mix )
{
  tp_service_init(1); // initialize with 1 interface
  lastRxDesc = NULL;
  packet_count =0;
  int frames;
  tp_packet_t msg0;
  tp_packet_t msg1;
  tp_packet_t msg2;

  //build one automatically
  tp_packet_build(&msg0,TP_PACKET_SENDCMD );
  tp_setCmd(&msg0, 0x43);
  tp_print_json(&msg0, printBuf);
  tp_service_feed_json(0, printBuf,strlen(printBuf));

  //msg1
  char* strMsg1 = "{ \"packetType\":\"Data\", \"sensorA\" : x6ca, \"sensorB\":898989, \"sensorName\":\"test name\" }";
  tp_service_feed_json(0, strMsg1,strlen(strMsg1));

  //msg2
  tp_packet_build(&msg2,TP_PACKET_DATA );
  tp_setSensorA(&msg2, 1738);
  tp_setSensorB(&msg2, 898989);
  tp_setSensorName(&msg2, "test name2");



  //feed 3
  feed_packet(0,&msg2);

  //verify 3 frames in fifo
  ASSERT_EQ(getFrameCount(0),3);


  //process to pull a frame and verify remaining count
  tp_service_process();
  ASSERT_EQ(packet_count,1);
  ASSERT_EQ(getFrameCount(0),2);
  ASSERT_EQ(lastRxDesc,TP_PACKET_SENDCMD);

  //process to pull a frame and verify remaining count
  tp_service_process();
  ASSERT_EQ(packet_count,2);
  ASSERT_EQ(getFrameCount(0),1);
  ASSERT_EQ(lastRxDesc,TP_PACKET_DATA);

  //process to pull a frame and verify remaining count
  tp_service_process();
  ASSERT_EQ(packet_count,3);
  ASSERT_EQ(getFrameCount(0),0);
  ASSERT_EQ(lastRxDesc,TP_PACKET_DATA);


  tp_service_teardown();
}

TEST(PolyService, ProcessTest )
{
  tp_service_init(1); // initialize with 1 interface
  lastRxDesc = NULL;

  int frames;
  tp_packet_t msg;
  packet_count =0;
  tp_packet_build(&msg,TP_PACKET_DATA );
  tp_setSensorA(&msg, 1738);
  tp_setSensorB(&msg, 898989);
  tp_setSensorName(&msg, "test name");

  //feed one frame
  feed_packet(0,&msg);

  //verify one frame in fifo
  frames = getFrameCount(0);
  ASSERT_EQ(frames,1);

  //process, this should pop frame from fifo
  tp_service_process();
  ASSERT_EQ(packet_count,1);
  ASSERT_EQ(lastRxDesc,TP_PACKET_DATA);


  frames = getFrameCount(0);
  ASSERT_EQ(frames,0);

  tp_service_teardown();
}

/**
  *@brief feed 3 messages to incoming buffer and verufy they process
  */
TEST(PolyService, multipleFrames )
{
  tp_service_init(1); // initialize with 1 interface
  lastRxDesc = NULL;
  packet_count =0;
  int frames;
  tp_packet_t msg0;
  tp_packet_t msg1;
  tp_packet_t msg2;


  //msg 0
  tp_packet_build(&msg0,TP_PACKET_SENDCMD );
  tp_setCmd(&msg0, 0x45);

  //msg1
  tp_packet_build(&msg1,TP_PACKET_DATA );
  tp_setSensorA(&msg1, 1738);
  tp_setSensorB(&msg1, 898989);
  tp_setSensorName(&msg1, "test name");

  //msg2
  tp_packet_build(&msg2,TP_PACKET_DATA );
  tp_setSensorA(&msg2, 1738);
  tp_setSensorB(&msg2, 898989);
  tp_setSensorName(&msg2, "test name2");

  //feed one
  feed_packet(0,&msg0);

  //feed 2
  feed_packet(0,&msg1);

  //feed 3
  feed_packet(0,&msg2);

  //verify 3 frames in fifo
  ASSERT_EQ(getFrameCount(0),3);


  //process to pull a frame and verify remaining count
  tp_service_process();
  ASSERT_EQ(packet_count,1);
  ASSERT_EQ(getFrameCount(0),2);
  ASSERT_EQ(lastRxDesc,TP_PACKET_SENDCMD);

  //process to pull a frame and verify remaining count
  tp_service_process();
  ASSERT_EQ(packet_count,2);
  ASSERT_EQ(getFrameCount(0),1);
  ASSERT_EQ(lastRxDesc,TP_PACKET_DATA);

  //process to pull a frame and verify remaining count
  tp_service_process();
  ASSERT_EQ(packet_count,3);
  ASSERT_EQ(getFrameCount(0),0);
  ASSERT_EQ(lastRxDesc,TP_PACKET_DATA);


  tp_service_teardown();
}

/**
  *@brief feed 3 messages to incoming buffer, but skip a byte in the second one. make sure it recovers
  */
TEST(PolyService, byteSkip )
{
  tp_service_init(1); // initialize with 1 interface
  lastRxDesc = NULL;
  packet_count =0;
  int frames;
  tp_packet_t msg0;
  tp_packet_t msg1;
  tp_packet_t msg2;


  //msg 0
  tp_packet_build(&msg0,TP_PACKET_SENDCMD );
  tp_setCmd(&msg0, 0x45);

  //msg1
  tp_packet_build(&msg1,TP_PACKET_DATA );
  tp_setSensorA(&msg1, 1738);
  tp_setSensorB(&msg1, 898989);
  tp_setSensorName(&msg1, "test name");

  //msg2
  tp_packet_build(&msg2,TP_PACKET_DATA );
  tp_setSensorA(&msg2, 1738);
  tp_setSensorB(&msg2, 898989);
  tp_setSensorName(&msg2, "test name2");

  //feed one
  feed_packet(0,&msg0);

  //feed 2 (skip byte[3])
  int len = poly_packet_pack_encoded(&msg2.mPacket,encoded);
  tp_service_feed(0,encoded,3);
  len = len-4;
  tp_service_feed(0,&encoded[4],len);

  //feed 3
  feed_packet(0,&msg2);

  //verify 3 frames in fifo (cob_fifo doesnt know one if bad. it just sees delimiters)
  ASSERT_EQ(getFrameCount(0),3);


  //process to pull a frame and verify remaining count. first one should be fine
  tp_service_process();
  ASSERT_EQ(packet_count,1);
  ASSERT_EQ(getFrameCount(0),2);
  ASSERT_EQ(lastRxDesc,TP_PACKET_SENDCMD);

  //process to pull a frame and verify remaining count. second process should not find a valid packet
  tp_service_process();
  ASSERT_EQ(packet_count,1);              //count should still be 1
  ASSERT_EQ(getFrameCount(0),1);          //frame should drop to 1
  ASSERT_EQ(lastRxDesc,TP_PACKET_SENDCMD); //last Rxd packet should still be msg0

  //process to pull a frame and verify remaining count. third one should be fine
  tp_service_process();
  ASSERT_EQ(packet_count,2);
  ASSERT_EQ(getFrameCount(0),0);
  ASSERT_EQ(lastRxDesc,TP_PACKET_DATA);


  tp_service_teardown();
}


/**
  *@brief feed 3 messages to incoming buffer, but add an incorrect byte in the encoded data
  */
TEST(PolyService, byteCorrupt )
{
  tp_service_init(1); // initialize with 1 interface
  lastRxDesc = NULL;
  packet_count =0;
  int frames;
  tp_packet_t msg0;
  tp_packet_t msg1;
  tp_packet_t msg2;


  //msg 0
  tp_packet_build(&msg0,TP_PACKET_SENDCMD );
  tp_setCmd(&msg0, 0x45);

  //msg1
  tp_packet_build(&msg1,TP_PACKET_DATA );
  tp_setSensorA(&msg1, 1738);
  tp_setSensorB(&msg1, 898989);
  tp_setSensorName(&msg1, "test name");

  //msg2
  tp_packet_build(&msg2,TP_PACKET_DATA );
  tp_setSensorA(&msg2, 1738);
  tp_setSensorB(&msg2, 898989);
  tp_setSensorName(&msg2, "test name2");

  //feed one
  feed_packet(0,&msg0);

  //feed 2 (skip byte[3])
  int len = poly_packet_pack_encoded(&msg2.mPacket,encoded);
  encoded[len -3] = 0xFF;  //throw gargabe into encoded data
  tp_service_feed(0,encoded,len);

  //feed 3
  feed_packet(0,&msg2);

  //verify 3 frames in fifo (cob_fifo doesnt know one if bad. it just sees delimiters)
  ASSERT_EQ(getFrameCount(0),3);


  //process to pull a frame and verify remaining count. first one should be fine
  tp_service_process();
  ASSERT_EQ(packet_count,1);
  ASSERT_EQ(getFrameCount(0),2);
  ASSERT_EQ(lastRxDesc,TP_PACKET_SENDCMD);

  //process to pull a frame and verify remaining count. second process should not find a valid packet
  tp_service_process();
  ASSERT_EQ(packet_count,1);              //count should still be 1
  ASSERT_EQ(getFrameCount(0),1);          //frame should drop to 1
  ASSERT_EQ(lastRxDesc,TP_PACKET_SENDCMD); //last Rxd packet should still be msg0

  //process to pull a frame and verify remaining count. third one should be fine
  tp_service_process();
  ASSERT_EQ(packet_count,2);
  ASSERT_EQ(getFrameCount(0),0);
  ASSERT_EQ(lastRxDesc,TP_PACKET_DATA);


  tp_service_teardown();
}

/**
  *@brief feed 3 messages to incoming buffer, but adds a zero byte (delimiter) in the data
  */
TEST(PolyService, addZero )
{
  tp_service_init(1); // initialize with 1 interface
  lastRxDesc = NULL;
  packet_count =0;
  int frames;
  tp_packet_t msg0;
  tp_packet_t msg1;
  tp_packet_t msg2;


  //msg 0
  tp_packet_build(&msg0,TP_PACKET_SENDCMD );
  tp_setCmd(&msg0, 0x45);

  //msg1
  tp_packet_build(&msg1,TP_PACKET_DATA );
  tp_setSensorA(&msg1, 1738);
  tp_setSensorB(&msg1, 898989);
  tp_setSensorName(&msg1, "test name");

  //msg2
  tp_packet_build(&msg2,TP_PACKET_DATA );
  tp_setSensorA(&msg2, 1738);
  tp_setSensorB(&msg2, 898989);
  tp_setSensorName(&msg2, "test name2");

  //feed one
  feed_packet(0,&msg0);

  //feed 2 (skip byte[3])
  int len = poly_packet_pack_encoded(&msg2.mPacket,encoded);
  encoded[len -3] = 0x00;  //throw gargabe into encoded data
  tp_service_feed(0,encoded,len);

  //feed 3
  feed_packet(0,&msg2);

  //verify 3 frames in fifo (cob_fifo doesnt know one if bad. it just sees delimiters)
  ASSERT_EQ(getFrameCount(0),4);


  //process to pull a frame and verify remaining count. first one should be fine
  tp_service_process();
  ASSERT_EQ(packet_count,1);
  ASSERT_EQ(getFrameCount(0),3);
  ASSERT_EQ(lastRxDesc,TP_PACKET_SENDCMD);

  //process to pull a frame and verify remaining count. second process should not find a valid packet
  tp_service_process();
  ASSERT_EQ(packet_count,1);              //count should still be 1
  ASSERT_EQ(getFrameCount(0),2);          //frame should drop to 1
  ASSERT_EQ(lastRxDesc,TP_PACKET_SENDCMD); //last Rxd packet should still be msg0

  //process to pull a frame and verify remaining count. second process should not find a valid packet
  tp_service_process();
  ASSERT_EQ(packet_count,1);              //count should still be 1
  ASSERT_EQ(getFrameCount(0),1);          //frame should drop to 1
  ASSERT_EQ(lastRxDesc,TP_PACKET_SENDCMD); //last Rxd packet should still be msg0

  //process to pull a frame and verify remaining count. third one should be fine
  tp_service_process();
  ASSERT_EQ(packet_count,2);
  ASSERT_EQ(getFrameCount(0),0);
  ASSERT_EQ(lastRxDesc,TP_PACKET_DATA);


  tp_service_teardown();
}

#endif
