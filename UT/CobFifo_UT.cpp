
#ifdef UNIT_TESTING_ENABLED



extern "C"{

#include "../cob_fifo.c"

}


#include <gtest/gtest.h>


cob_fifo_t cobFifo;


#define FRAME_0_LEN 21
#define FRAME_1_LEN 12
uint8_t testFrame0[FRAME_0_LEN] = {0x00, 0x2D, 0x41, 0x00, 0x02, 0x03, 0x43, 0x00, 0x0C, 0x00, 0x2D, 0x20, 0x0C, 0x59, 0x59, 0x5A, 0x00, 0x0C, 0x00, 0x0C, 0x00};
uint8_t testFrame1[FRAME_1_LEN] = {0x20, 0x0C, 0x59, 0x59, 0x5A, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x3D, 0x62};

uint8_t testComp0[FRAME_0_LEN];
uint8_t testComp1[FRAME_1_LEN];

TEST(CobFifo_Test, pushframe )
{
  cob_fifo_t cobFifo;
  int nextLen;
  cob_fifo_init(&cobFifo,128);

  cob_fifo_push_frame(&cobFifo, testFrame0, FRAME_0_LEN);  //encoding adds one byte

  ASSERT_EQ(cobFifo.mCount, FRAME_0_LEN + 2); //1 byte of Overhead and one delimiter
  ASSERT_EQ(cobFifo.mNextLen,FRAME_0_LEN +2); //location of delimiter



  cob_fifo_deinit(&cobFifo);
}

TEST(CobFifo_Test, popframe )
{
  cob_fifo_t cobFifo;
  int nextLen;
  cob_fifo_init(&cobFifo,128);

  cob_fifo_push_frame(&cobFifo, testFrame0, FRAME_0_LEN);  //encoding adds one byte
  cob_fifo_pop_frame(&cobFifo, testComp0, FRAME_0_LEN);

  for(int i =0; i < FRAME_0_LEN; i++)
  {
    ASSERT_EQ(testComp0[i],testFrame0[i]);
  }

  ASSERT_EQ(cobFifo.mCount, 0);
  ASSERT_EQ(cobFifo.mFrameCount, 0);
  ASSERT_EQ(cobFifo.mNextLen, 0);

  cob_fifo_deinit(&cobFifo);
}



TEST(CobFifo_Test, multiframes )
{
  cob_fifo_t cobFifo;
  int nextLen;
  int decodedLen;
  cob_fifo_init(&cobFifo,128);

  cob_fifo_push_frame(&cobFifo, testFrame0, FRAME_0_LEN);  //encoding adds one byte
  cob_fifo_push_frame(&cobFifo, testFrame1, FRAME_1_LEN);

  nextLen = cob_fifo_get_next_len(&cobFifo);
  ASSERT_EQ(nextLen,FRAME_0_LEN +2 );
  ASSERT_EQ(cobFifo.mCount, FRAME_0_LEN + FRAME_1_LEN + 4); // 2 ohb + 2 delimiters
  ASSERT_EQ(cobFifo.mNextLen,FRAME_0_LEN+2);

  decodedLen = cob_fifo_pop_frame(&cobFifo, testComp0, FRAME_0_LEN);
  ASSERT_EQ(decodedLen,FRAME_0_LEN);

  for(int i =0; i < FRAME_0_LEN; i++)
  {
    ASSERT_EQ(testComp0[i],testFrame0[i]);
  }

  ASSERT_EQ(cobFifo.mCount, FRAME_1_LEN+2);
  ASSERT_EQ(cobFifo.mNextLen,FRAME_1_LEN+2);

  for(int i =0; i < FRAME_1_LEN; i++)
  {
    ASSERT_EQ(testComp0[i],testFrame0[i]);
  }

  decodedLen = cob_fifo_pop_frame(&cobFifo, testComp1, FRAME_1_LEN);
  ASSERT_EQ(decodedLen,FRAME_1_LEN);

  ASSERT_EQ(cobFifo.mCount, 0);
  ASSERT_EQ(cobFifo.mNextLen,0);

  cob_fifo_deinit(&cobFifo);
}





#endif
