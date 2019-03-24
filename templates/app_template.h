/**
  *@file app_${proto.name.lower()}.cpp
  *@brief generated protocol source code
  *@author make_protocol.py
  *@date ${proto.genTime}
  */

#include "${proto.fileName}.h"


/**
  *@brief Initialize the packet service
  */
%if proto.genUtility:
void app_${proto.name.lower()}_init(const char* port, int baud);
%else:
void app_${proto.name.lower()}_init();
%endif

/**
  *@brief process the data for the packet service
  */
void app_${proto.name.lower()}_process();
