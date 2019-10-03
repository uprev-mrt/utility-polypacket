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

  MRT_MUTEX_CREATE(spool->mMutex);

  //set default values
  spool->mMaxEntries = len;
  spool->mEntries = (spool_entry_t*)malloc(sizeof(spool_entry_t) * len);
  spool->mCount = 0;
  spool->mReadyCount =0;
  spool->mWaitingCount =0;
  spool->mTimeOut = 2500;
  spool->mMaxRetries = 5;
  spool->mSuccessfulMessages =0;
  spool->mFailedMessages =0;

  //set all entries to free
  for(int i=0; i < len; i++)
  {
    spool->mEntries[i].mState = ENTRY_STATE_FREE;
    spool->mEntries[i].mTrash = false;
    spool->mEntries[i].mAttempts = 0;
    spool->mEntries->mTimeOut = 0;
    poly_packet_build(&spool->mEntries[i].mPacket, NULL, true);
  }

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

/**
  *@brief gets next entry in spool with mState == free, if none are free and there is a lower priority, it will remove that to free the spot
  *@param c - pointer to spool_t
	*@param priority - priority threshold to remove
  *@return index of entry, or -1 if none are found
  */
static inline int poly_spool_get_next_free(poly_spool_t* spool, uint8_t priority )
{
  int idx = ENTRY_NONE;
  int8_t lowestPriority = priority;	//lowest priority
  int lowestPriorityIdx = -1;				// index of lowest priority

  for(int i=0; i < spool->mMaxEntries; i++)
  {

    if(spool->mEntries[i].mPacket.mPriority < lowestPriority)
    {
      lowestPriority = spool->mEntries[i].mPacket.mPriority;
      lowestPriorityIdx = i;
    }

    if(spool->mEntries[i].mState == ENTRY_STATE_FREE)
    {
      idx =i;
      break;
    }
  }

  //If there is no entry available (i.e. spool is full), remove a lower priority entry if one exists
  if((idx == ENTRY_NONE) && (lowestPriorityIdx > -1))
	{
    idx = lowestPriorityIdx;

    //destroy whatever is in the entry
    poly_packet_clean(&spool->mEntries[idx].mPacket);

    spool->mCount--;
    switch (spool->mEntries[idx].mState)
    {
      case ENTRY_STATE_WAITING:
        spool->mWaitingCount--;
        break;
      case ENTRY_STATE_READY:
        spool->mReadyCount--;
        break;
    }
	}

  return idx;
}



void poly_spool_deinit(poly_spool_t* spool)
{
  MRT_MUTEX_DELETE(spool->mMutex);
  spool->mMaxEntries = 0;
  spool->mReadyCount =0;
  spool->mCount =0;
  for(int i =0 ; i < spool->mMaxEntries; i++)
  {
    if((spool->mEntries[i].mState != ENTRY_STATE_FREE) || (spool->mEntries[i].mTrash))
    {
      poly_packet_clean(&spool->mEntries[i].mPacket);
    }
  }
  free(spool->mEntries);

}


spool_status_e poly_spool_push(poly_spool_t* spool, poly_packet_t* packet )
{
  MRT_MUTEX_LOCK(spool->mMutex);
  spool_status_e status;

  //get next free Entry
  int idx = poly_spool_get_next_free(spool, packet->mPriority);
  spool_entry_t* entry;

  //make sure we arent full
  if(spool->mCount >= spool->mMaxEntries)
  {
    MRT_MUTEX_UNLOCK(spool->mMutex);
    return SPOOL_OVERFLOW;
  }

  if(idx > ENTRY_NONE)
  {
    entry = &spool->mEntries[idx];

    //If entry still has trash, clean it
    if(entry->mTrash)
      poly_packet_clean(&entry->mPacket);

    //reset entry properties
    entry->mTimeOut = 0;
    entry->mState = ENTRY_STATE_READY;
    entry->mAttempts = 0;

    //copy in packet data
    memcpy((void*) &entry->mPacket, (void*)packet, sizeof(poly_packet_t));
	
	if(packet->mReusable)
		poly_packet_reset(packet);

    //increment counts
    spool->mCount++;
    spool->mReadyCount++;  //decrement ready count
  }

  MRT_MUTEX_UNLOCK(spool->mMutex);
  return SPOOL_OK;
}


spool_status_e poly_spool_pop(poly_spool_t* spool, poly_packet_t* packet)
{
  MRT_MUTEX_LOCK(spool->mMutex);
  spool_status_e status;

  //get next ready packet
  int idx = poly_spool_get_next(spool, ENTRY_STATE_READY);
  spool_entry_t* entry;
  uint16_t newToken =0;

  if(idx > ENTRY_NONE)
  {
    entry = &spool->mEntries[idx];


    if((entry->mPacket.mAckType == ACK_TYPE_TOKEN) && ((entry->mPacket.mHeader.mToken & SPOOL_TOKEN_ACK_REQ) ==0))
    {

      //reset the timeout counter to the spools timeout setting
      entry->mTimeOut = spool->mTimeOut;

      entry->mState = ENTRY_STATE_WAITING;
      spool->mWaitingCount ++;
      entry->mAttempts++;

      //copy the packet out
      memcpy((void*)packet, (void*)&entry->mPacket, sizeof(poly_packet_t) );

    }
    else
    {
      //If we dont need an ack, then we are done with this packet
      entry->mState = ENTRY_STATE_FREE;
      spool->mCount--;

      //copy the packet out
      memcpy((void*)packet, (void*)&entry->mPacket, sizeof(poly_packet_t) );

      //Mark the entry for trash collection
      entry->mTrash = true;

    }

    spool->mReadyCount--;

    status = SPOOL_OK;

  }
  else
  {
    status = SPOOL_UNDERFLOW;
  }

  MRT_MUTEX_UNLOCK(spool->mMutex);
  return status;
}


bool poly_spool_ack(poly_spool_t* spool, poly_packet_t* response)
{
  bool match = false;
  int i;
  uint16_t ackToken = response->mHeader.mToken;
  //if the token is 0 or is an ack request, we ignore it
  if((ackToken & SPOOL_TOKEN_ACK_REQ) == 0)
  {
    return false;
  }
  ackToken &=SPOOL_TOKEN_MASK;


  MRT_MUTEX_LOCK(spool->mMutex);

  //Check each entry in the state ENTRY_STATE_WAITING to see if the token matches
  for( i=0;i < spool->mMaxEntries; i++)
  {
    //if entry is waiting, and the token matches after masking off the request bit
    if((spool->mEntries[i].mState == ENTRY_STATE_WAITING) && (ackToken == spool->mEntries[i].mPacket.mHeader.mToken))
    {
      //match found, free entry
      spool->mEntries[i].mState = ENTRY_STATE_FREE;

      spool->mEntries[0].mTrash = true;
      spool->mWaitingCount--;
      spool->mCount--;

      match = true;
      break;
    }
  }

  MRT_MUTEX_UNLOCK(spool->mMutex);

  //if we found a match and the entry has a callback, call it
  if(match && (spool->mEntries[i].mPacket.f_mAckCallback != NULL))
  {
    spool->mEntries[i].mPacket.f_mAckCallback(response);
  }

  return match;
}


void poly_spool_tick(poly_spool_t* spool, int ms)
{
  MRT_MUTEX_LOCK(spool->mMutex);
  spool_entry_t* entry;

  for(int i=0; i < spool->mMaxEntries; i++)
  {
    entry = &spool->mEntries[i];

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
          //destroy the packet data
          entry->mTrash = true;
          entry->mState = ENTRY_STATE_FREE;
          spool->mCount--;
          spool->mWaitingCount--;
          spool->mFailedMessages++;
        }
      }
    }
  }
  MRT_MUTEX_UNLOCK(spool->mMutex);
}
