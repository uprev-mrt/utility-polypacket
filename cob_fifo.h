/**
  *@file cob_fifo.h
  *@brief header for fifo that implements Consisten Overhead ByteStuffing
  *@author Jason Berger
  *@date 03/26/2019
  *
  *@info https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
  */

/**
  *
  *Consistent Overhead Byte Stuffing (COBS) is an algorithm for encoding data bytes that results
  *in efficient, reliable, unambiguous packet framing regardless of packet content, thus making it
  *easy for receiving applications to recover from malformed packets. It employs a particular byte value,
  *typically zero, to serve as a packet delimiter (a special value that indicates the boundary between packets).
  *When zero is used as a delimiter, the algorithm replaces each zero data byte with a non-zero value so that no
  *zero data bytes will appear in the packet and thus be misinterpreted as packet boundaries.
  */

#pragma once

#include <stdint.h>
#include "Platforms/Common/mrt_platform.h"

typedef enum{
  COB_FIFO_OK=0,
  COB_FIFO_OVERFLOW = -1,
  COB_FIFO_UNDERFLOW = -2
}cob_fifo_status_e;


typedef struct {
    uint8_t *  mBuffer;   //pointer to data buffer
    int mHead;            //next address to write to
    int mTail;            //next address to read from
    int mCount;           //number of bytes in fifo
    int mFrameCount;     //number of frames in the fifo
    int mMaxLen;          //max number of bytes in fifo
    int mNextLen;        //length of next frame in fifo (including delim)
    MRT_MUTEX_TYPE mMutex;        //lock for fifo that is accessed by multiple threads
} cob_fifo_t;

/**
  *@brief initializes the Consistent Overhead ByteStuffing
  *@param ptr to fifo
  *@param len number of bytes fifo cna hold
  */
void cob_fifo_init(cob_fifo_t* fifo, int len);

/**
  *@brief dinitializes the fifo
  *@param fifo ptr to the fifo
  */
void cob_fifo_deinit(cob_fifo_t* fifo);

/**
  *@brief pushes byte to the fifo without any manipulation
  *@param fifo ptr to the fifo
  *@param data data to be pushed to the fifo
  *@return status of fifo operation
  */
cob_fifo_status_e cob_fifo_push(cob_fifo_t* fifo, uint8_t data);

/**
  *@brief pops byte from the fifo without any manipulation
  *@param fifo ptr to the fifo
  *@param data ptr to data to pop to
  *@return status of fifo operation
  */
cob_fifo_status_e cob_fifo_pop(cob_fifo_t* fifo, uint8_t* data);

/**
  *@brief pushes bytes to the fifo without any manipulation
  *@param fifo ptr to the fifo
  *@param data data to be pushed to the fifo
  *@return status of fifo operation
  */
cob_fifo_status_e cob_fifo_push_buf(cob_fifo_t* fifo, const uint8_t* data, int len);

/**
  *@brief pops byte from the fifo without any manipulation
  *@param fifo ptr to the fifo
  *@param data ptr to data to pop to
  *@return status of fifo operation
  */
cob_fifo_status_e cob_fifo_pop_buf(cob_fifo_t* fifo, uint8_t* data, int len);

/**
  *@brief pushes complete frame to the fifo and encodes it in the process
  *@param fifo ptr to the fifo
  *@param data raw frame to be placed in the fifo
  *@param len number of bytes in the raw data frame
  *@return status of fifo operation
  */
cob_fifo_status_e cob_fifo_push_frame(cob_fifo_t* fifo, const uint8_t* data, int len);

/**
  *@brief pops complete frame from the fifo and decodes in the process
  *@param fifo ptr to the fifo
  *@param data ptr to store decoded frame
  *@param len maximum number of bytes available to write to
  *@return length of decoded frame in bytes
  *@return zero if no frame is available
  */
int cob_fifo_pop_frame(cob_fifo_t* fifo, uint8_t* data, int len);

/**
  *@brief gets the length of th next frame (including delimiter) in the fifo
  *@param fifo ptr to the fifo
  *@return length of encoded frame and delimiter
  *@return zero if no frames are available
  */
int cob_fifo_get_next_len(cob_fifo_t* fifo);
