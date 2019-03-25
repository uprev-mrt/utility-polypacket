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

void sig_exit();
void catch_sigterm();

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


void sig_exit()
{
  printf("closing!\n");
  app_end();
  exit(0);
}

void catch_sigterm()
{
    static struct sigaction _sigact;

    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = sig_exit;
    _sigact.sa_flags = SA_SIGINFO;

    sigaction(SIGINT, &_sigact, NULL);
}
