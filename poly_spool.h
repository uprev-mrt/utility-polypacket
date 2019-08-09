/**
  *@file poly_spool.h
  *@brief ack/retry spool for poly packet
  *@author Jason Berger
  *@date 03/22/2019
  */

#pragma once


#include <stdint.h>
#include <stdbool.h>
#include "poly_packet.h"

typedef void (*spool_ack_cb)(poly_packet_t* response);

#define ENTRY_NONE -1

#define SPOOL_TOKEN_MASK 0x7FFF
#define SPOOL_TOKEN_ACK_REQ 0x8000

//State of entries in the spool
typedef enum{
  ENTRY_STATE_FREE,     //Entry is unused
  ENTRY_STATE_READY,    //Entry is ready to send
  ENTRY_STATE_WAITING   //Entry was sent and is waiting for an ack or to timeout for a retry
} spool_entry_state_e;


//Status of spool actions
typedef enum{
  SPOOL_OK = 0,           //Everything is ok
  SPOOL_OVERFLOW ,     //Spool is full
  SPOOL_UNDERFLOW ,    //Spool is empty
  SPOOL_ERROR         //Misc errors
} spool_status_e;


typedef struct{
    spool_entry_state_e mState;     //current state of entry
    int mTimeOut;                   //current state of timeout counter
    int mAttempts;                  //number of attempts to send this packet so far
    poly_packet_t mPacket;          //actual packet being sent
    bool mTrash;                    //if entry was set to free but not cleaned yet
  } spool_entry_t;

  typedef struct{
    spool_entry_t* mEntries;        //entries in spool
    int mMaxEntries;                //max number of entries in spool
    int mCount;                     //current number of entries in spool
    int mReadyCount;                //number of entries in the READY state
    int mWaitingCount;              //number of entries waiting for acks
    int mTimeOut;                   //timeout for entries
    int mMaxRetries;                //maximum attempts for each packet before giving up
    MRT_MUTEX_TYPE mMutex;           //lock bit/mutex //TODO add mutex support when available
    //used for debugging
    int mSuccessfulMessages;        //successful messages
    int mFailedMessages;            //failed
  }poly_spool_t;

/**
  *@brief initialize a spool
  *@param spool ptr to spool to initialize
  *@param len number of entries to keep in spool
  */
void poly_spool_init(poly_spool_t* spool, int len);

/**
  *@brief deinit the spool
  *@param spool spool to deinit
  */
void poly_spool_deinit(poly_spool_t* spool);

/**
  *@brief pushes a new packet onto the spool
  *@param spool ptr to spool
  *@param packet packet to add to spool
  *@return SPOOL_OK push was successful
  *@return SPOOL_OVERFLOW spool was already full
  */
spool_status_e poly_spool_push(poly_spool_t* spool, poly_packet_t* packet );

/**
  *@brief pulls
  *@param spool ptr to spool
  *@param packet packet to add to spool
  *@return SPOOL_OK push was successful
  *@return SPOOL_UNDERFLOW spool is empty or no packets are in the READY state
  */
spool_status_e poly_spool_pop(poly_spool_t* spool, poly_packet_t* packet);

/**
  *@brief passes an ack token to the spool to check for matches
  *@param spool ptr to spool
  *@param response response packet to check token of
  *@return true if matching request token was found
  *@return false if no match is found
  */
bool poly_spool_ack(poly_spool_t* spool, poly_packet_t* response);

/**
  *@brief increments the time for the spool so it can check timeouts
  *@param spool ptr to spool
  *@param ms time in milliseconds since the last update
  */
void poly_spool_tick(poly_spool_t* spool, int ms);
