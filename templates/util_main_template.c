/**
 * \file  lib_front.c
 *
 * \brief Front panel access utility.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "app_${proto.name.lower()}.h"



int main(int argc, char* argv[])
{

  /* Initialize app/service */
  app_${proto.name.lower()}_init("/dev/ttyS16", 9600);

  while(1)
  {
    app_${proto.name.lower()}_process();
  }

  return 0;
}
