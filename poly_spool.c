/**
  *@file poly_spool.c
  *@brief ack/retry spool for poly packet
  *@author Jason Berger
  *@date 03/22/2019
  */

#include "poly_spool.h"
#include <assert.h>


void poly_spool_init(poly_spool_t* spool, int len)
{
  SPOOL_LOCK;

  //set default values
  spool->mMaxEntries = len;
  spool->mEntries = (spool_entry_t*)malloc(sizeof(spool_entry_t) * len);
  spool->mCount = 0;
  spool->mReadyCount =0;
  spool->mWaitingCount =0;
  spool->mTimeOut = 400; //Doherty Threshold?
  spool->mMaxRetries = 10;
  spool->mLock  = false;
  spool->mSuccessfulMessages =0;
  spool->mFailedMessages =0;

  //set all entries to free
  for(int i=0; i < len; i++)
  {
    spool->mEntries[i].mState = ENTRY_STATE_FREE;
  }

  SPOOL_UNLOCK;
}

/**
  *@brief gets next entry in spool with mState == state
  *@param spool - pointer to spool
  *@param state - state to look for
  *@return index of entry if found
  *@return -1 if none are found
  */
static inline int poly_spool_get_next(poly_spool_t* spool, spool_entry_state_e state)
{
  int idx = -1;
  for(int i=0; i < spool->mMaxEntries; i++)
  {
    if(spool->mEntries[i].mState == state)
    {
      idx =i;
      break;
    }
  }

  return idx;
}


void poly_spool_deinit(poly_spool_t* spool)
{
  SPOOL_LOCK;
  spool->mMaxEntries = 0;
  spool->mReadyCount =0;
  spool->mCount =0;
  free(spool->mEntries);
  SPOOL_UNLOCK;
}


spool_status_e poly_spool_push(poly_spool_t* spool, poly_packet_t* packet, spool_entry_ack_type ackType , spool_ack_cb cb )
{
  SPOOL_LOCK;
  spool_status_e status;

  //get next free Entry
  int idx = poly_spool_get_next(spool, ENTRY_STATE_FREE);
  spool_entry_t* entry;

  //make sure we arent full
  if(spool->mCount >= spool->mMaxEntries)
  {
    SPOOL_UNLOCK;
    return SPOOL_OVERFLOW;
  }

  if(idx > ENTRY_NONE)
  {
    //reset entry properties
    entry = &spool->mEntries[idx];
    entry->mTimeOut = 0;
    entry->mState = SPOOL_STATE_READY;
    entry->mAttempts = 0;

    //copy in packet data
    memcpy((void*)entry->mPacket, (void*)packet, sizeof(poly_packet_t));

    //increment counts
    spool->mCount++;
    spool->mReadyCount++;  //decrement ready count
  }

  SPOOL_UNLOCK;
  return SPOOL_OK;
}


spool_status_e poly_spool_pop(poly_spool_t* spool, poly_packet_t* packet)
{
  SPOOL_LOCK;
  spool_status_e status;

  //get next ready packet
  int idx = poly_spool_get_next(spool, ENTRY_STATE_READY);
  spool_entry_t* entry;
  uint16_t newToken =0;

  if(idx > ENTRY_NONE)
  {
    entry = spool->mEntries[idx];


    if(entry->mPacket.mAckType == ACK_TYPE_TOKEN)
    {
      //if ack type is token, then we generate a randome token and make sure it wont be 0 without the ack flag
      while(newToken ==0)
      {
        newToken = rand() & SPOOL_TOKEN_MASK;
      }

      //set the token with the ack request flag so the receiver knows to ack
      entry->mPacket.mHeader.mToken = newToken | SPOOL_TOKEN_ACK_REQ;

      //reset the timeout counter to the spools timeout setting
      entry->mTimeOut = spool->mTimeOut;

      entry->mState = ENTRY_STATE_WAITING;
      spool->mWaitingCount ++;
      entry->mAttempts++;
    }
    else
    {
      //If we dont need to an ack, then we are done with this packet
      entry->mState = ENTRY_STATE_FREE;
      spool->mCount--;
    }

    spool->mReadyCount--;

    //copy the packet out
    memcpy((void*)packet, (void*)entry->mPacket, sizeof(poly_packet_t) );
    status = SPOOL_OK;

  }
  else
  {
    status = SPOOL_UNDERFLOW
  }

  SPOOL_UNLOCK;
  return status;
}


bool poly_spool_ack(poly_spool_t* spool, poly_packet_t* response)
{
  bool match = false;
  int i;
  //if the token is 0 or is an ack request, we ignore it
  if((ack_token == 0) || (ack_token & SPOOL_TOKEN_ACK_REQ))
  {
    return false;
  }

  SPOOL_LOCK;

  //Check each entry in the state ENTRY_STATE_WAITING to see if the token matches
  for( i=0;i < spool->mMaxEntries; i++)
  {
    //if entry is waiting, and the token matches after masking off the request bit
    if((spool->mEntries[i].mState == ENTRY_STATE_WAITING) && (ackToken == (spool->mEntries[i].mPacket.mHeader.mToken & SPOOL_TOKEN_MASK )))
    {
      //match found, free entry
      spool->mEntries[i].mState = ENTRY_STATE_FREE;
      spool->mWaitingCount--;
      spool->mCount--;

      match = true;
      break;
    }
  }

  SPOOL_UNLOCK;

  //if we found a match and the entry has a callback, call it
  if(match && (spool->mEntries[i].mPacket.f_mAckCallback != NULL))
  {
    spool->mEntries[i].mPacket.f_mAckCallback(response);
  }

  return match;
}


void poly_spool_tick(poly_spool_t* spool, int ms)
{
  SPOOL_LOCK;
  spool_entry_t* entry;

  for(int i=0; i < spool->mSize; i++)
  {
    entry = &c->mEntries[i];

    //time only matters to waiting entries
    if(entry->mState == ENTRY_STATE_WAITING)
    {
      //subtract the time from the entries
      entry->mTimeOut -= ms;

      //If we go to zero, the packet has timed out
      if(entry->mTimeOut <= 0)
      {
        //if we havent maxxed out retries set it to ready and try again
        if(entry->mAttempts < spool->mMaxRetries)
        {
          entry->mState = ENTRY_STATE_READY;
          spool->mReadyCount++;
          spool->mWaitingCount--;
        }
        else //if we max out our retries, free the slot and note a failed message
        {
          entry->mState = ENTRY_STATE_FREE;
          spool->mCount--;
          spool->mWaitingCount--;
          spool->mFailedMessages++;
        }
      }
    }
  }
  SPOOL_UNLOCK;
}
