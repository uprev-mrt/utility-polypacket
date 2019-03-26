/**
  *@file app_${proto.name.lower()}.h
  *@brief generated protocol source code
  *@author make_protocol.py
  *@date ${proto.genTime}
  */

#include "${proto.fileName}.h"
%if not proto.genUtility:
#include "Platforms/Common/mrt_platform.h"
%endif


/**
  *@brief Initialize the packet service
  */
%if proto.genUtility:
void app_${proto.name.lower()}_init(const char* port, int baud);
%else:
void app_${proto.name.lower()}_init(mrt_uart_handle_t uart_handle);
%endif

/**
  *@brief ends service
  */
void app_${proto.name.lower()}_end();

/**
  *@brief process the data for the packet service
  */
void app_${proto.name.lower()}_process();
