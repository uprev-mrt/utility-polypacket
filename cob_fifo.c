/**
  *@file cob_fifo.c
  *@brief source for fifo that implements Consisten Overhead ByteStuffing
  *@author Jason Berger
  *@date 03/26/2019
  *
  *@info https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
  */

#include "cob_fifo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define COB_FIFO_LOCK //fifo->mLock =1//while(fifo->lock){delay_ms(1);} fifo->lock = 1
#define COB_FIFO_UNLOCK //fifo->mLock = 0

void cob_fifo_init(cob_fifo_t* fifo, int len)
{
  fifo->mBuffer = (uint8_t*) malloc(len);
  fifo->mHead = 0;
  fifo->mTail = 0;
  fifo->mCount = 0;
  fifo->mLock = 0;
  fifo->mFrameCount = 0;
  fifo->mNextLen = 0;
  fifo->mMaxLen = len;
}


void cob_fifo_deinit(cob_fifo_t* fifo)
{
  free(fifo->mBuffer);
}


cob_fifo_status_e cob_fifo_push(cob_fifo_t* fifo, uint8_t data)
{
  COB_FIFO_LOCK;

    // next is where head will point to after this write.
    int next = fifo->mHead + 1;

    if (next > fifo->mMaxLen)
    {
        next = 0;
    }
    if (next == fifo->mTail) // check if circular buffer is full
    {
		COB_FIFO_UNLOCK;
        return COB_FIFO_OVERFLOW;
    }

    fifo->mCount++;

    //With byte Stuffing the only zero bytes are frame boundaries
    if(data == 0 )
    {
      fifo->mFrameCount++;

      //if next frame is not set yet
      if(fifo->mNextLen == 0)
      {
        fifo->mNextLen = fifo->mCount;
      }
    }

    fifo->mBuffer[fifo->mHead] = data;

    fifo->mHead = next;            // head to next data offset.

    COB_FIFO_UNLOCK;

    return COB_FIFO_OK;  // return success to indicate successful push.
}


cob_fifo_status_e cob_fifo_pop(cob_fifo_t* fifo, uint8_t* data)
{
  COB_FIFO_LOCK;
    // if the head isn't ahead of the tail, we don't have any characters
    if (fifo->mHead == fifo->mTail) // check if circular buffer is empty
    {
		COB_FIFO_UNLOCK;
        return COB_FIFO_UNDERFLOW;          // and return with an error
    }

    // next is where tail will point to after this read.
    int next = fifo->mTail + 1;
    if (next > fifo->mMaxLen)
    {
        next = 0;
    }

    *data = fifo->mBuffer[fifo->mTail];

    if(*data == 0)
    {
      fifo->mFrameCount--;
      if(fifo->mNextLen >=0)
        fifo->mNextLen--;
    }

    fifo->mTail = next;             // tail to next data offset.

    if(fifo->mCount > 0)
    {
        fifo->mCount--;
    }
    COB_FIFO_UNLOCK;

    return COB_FIFO_OK;  // return success to indicate successful push.
}


cob_fifo_status_e cob_fifo_push_buf(cob_fifo_t* fifo, const uint8_t* data, int len)
{
  cob_fifo_status_e result;

  for(int i=0; i < len; i++)
  {
    result = cob_fifo_push(fifo,data[i]);
  }
  return result;
}


cob_fifo_status_e cob_fifo_pop_buf(cob_fifo_t* fifo, uint8_t* data, int len)
{
  cob_fifo_status_e result;

  for(int i=0; i < len; i++)
  {
    result = cob_fifo_pop(fifo, &data[i]);
  }
  return result;
}


cob_fifo_status_e cob_fifo_push_frame(cob_fifo_t* fifo, const uint8_t* data, int len)
{
  int maxEncodedLen = len + ((len+255)/254);
  if((maxEncodedLen + fifo->mCount) > fifo->mMaxLen)
    return COB_FIFO_OVERFLOW;

  int actualLen;
  uint8_t encoded[maxEncodedLen +1];
  const uint8_t* ptr = data;
  uint8_t* dst = encoded;
  const uint8_t *start = dst, *end = ptr + len;
	uint8_t code, *code_ptr; /* Where to insert the leading count */


  //TODO optimize
  code_ptr = dst++;
  code = 1;
	while (ptr < end)
  {
		if (code != 0xFF)
    {
			uint8_t c = *ptr++;
			if (c != 0)
      {
				*dst++ = c;
				code++;
				continue;
			}
		}
    *code_ptr = code;
    code_ptr = dst++;
    code = 1;

	}
  *code_ptr++ = code;
  *dst++ = 0; //add delimiter

  actualLen = (int)(dst-start);

  cob_fifo_push_buf(fifo,encoded,actualLen);

  return COB_FIFO_OK;
}


int cob_fifo_pop_frame(cob_fifo_t* fifo, uint8_t* data, int len)
{
  if(fifo->mFrameCount ==0)
    return 0;

  int encodedLen = fifo->mNextLen;
  uint8_t encoded[encodedLen];

  cob_fifo_pop_buf(fifo,encoded,encodedLen);

  //TODO optimize
  uint8_t* ptr = encoded;
  uint8_t* dst = data;
  const uint8_t *start = dst, *end = ptr + encodedLen;
	uint8_t code = 0xFF, copy = 0;

	for (; ptr < end; copy--)
  {
		if (copy != 0)
    {
			*dst++ = *ptr++;
		}
    else
    {
			if (code != 0xFF)
				*dst++ = 0;
			copy = code = *ptr++;
			if (code == 0)
				break; /* Source length too long */
		}
	}

  cob_fifo_find_next_len(fifo);

	return dst - start;
}

int cob_fifo_find_next_len(cob_fifo_t* fifo)
{
  COB_FIFO_LOCK;
  int i;

  if(fifo->mFrameCount ==0)
  {
    COB_FIFO_UNLOCK;
    fifo->mNextLen = 0;
    return 0;
  }

  int addr = fifo->mTail;

  for( i=1; i < fifo->mCount; i++)
  {
    if(fifo->mBuffer[addr++] == 0)
    {
      break;
    }

    if(addr == fifo->mMaxLen)
      addr=0;
  }

  fifo->mNextLen = i;

  COB_FIFO_UNLOCK;
  return i;
}
