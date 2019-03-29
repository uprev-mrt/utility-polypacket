/**
  *@file app_sample.h
  *@brief generated protocol source code
  *@author make_protocol.py
  *@date 03/29/19
  */

#include "SampleService.h"


/**
  *@brief Initialize the packet service
  */
void app_sample_init(const char* port, int baud);

/**
  *@brief ends service
  */
void app_sample_end();

/**
  *@brief process the data for the packet service
  */
void app_sample_process();