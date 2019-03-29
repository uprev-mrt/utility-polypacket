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
#include <signal.h>

#include "app_sample.h"

void sig_exit();
void catch_sigterm();
void run_sp_args(int argc, char* argv[],int idx);
void printUsage(const char* name);






int main(int argc, char* argv[])
{
  int opt;
  void catch_sigterm(); /* so we can exit gracefulyl with ctrl+c  */

  /* Look for options. */
    while ((opt = getopt(argc, argv, "mp")) != -1)
    {
        switch (opt)
        {
            case 'm':
            {
              break;
            }
            default:
            {
                printUsage(argv[0]);
                break;
            }
        }
    }

    /* Initialize app/service */
    app_sample_init("/dev/ttyUSB0", 9600);

    /* Parse non-flag options */
    if (optind < argc)
      run_sp_args(argc, argv,optind);


  while(1)
  {
    app_sample_process();
    sleep(0.001);
  }

  return 0;
}



/*******************************************************************************
    Arguments to send messages directly
*******************************************************************************/

void run_sp_args(int argc, char* argv[], int idx)
{
  char strArgs[128]={0};
  int paramCount = argc - (idx +1);

  /* all fields */
  uint8_t field_cmd; //
  int16_t field_sensorA; //Value of Sensor A
  int field_sensorB; //Value of Sensor B
  char field_sensorName[32]; //Name of sensor

  if(argc < idx)
    return;

  if(argc > 2)
  {
    //concat args so we can sscanf easily
    for (int i = idx+1; i < argc; ++i)
    {
      strcat(strArgs, argv[i]);
      strcat(strArgs, " ");
    }
  }

  if(!strcasecmp(argv[1], "ping"))
  {
    sp_sendPing(0);
  }
  else if((!strcasecmp(argv[idx], "SendCmd")) && (paramCount == SP_PACKET_SENDCMD->mFieldCount))       //Message to set command in node
  {
    printf("%s\n",strArgs );
    sscanf(strArgs, "%i " , &field_cmd );
    sp_sendSendCmd(0, field_cmd );
  }
  else if((!strcasecmp(argv[idx], "GetData")) && (paramCount == SP_PACKET_GETDATA->mFieldCount))       //
  {
    printf("%s\n",strArgs );
    sp_sendGetData(0);
  }
  else if((!strcasecmp(argv[idx], "Data")) && (paramCount == SP_PACKET_DATA->mFieldCount))       //Message containing data from sensor
  {
    printf("%s\n",strArgs );
    sscanf(strArgs, "%i %i %s " , &field_sensorA , &field_sensorB , field_sensorName );
    sp_sendData(0, field_sensorA , field_sensorB , field_sensorName );
  }
  else
  {
    printUsage(argv[0]);
  }
}

void printUsage(const char* name)
{
  printf("Usage: %s [OPTIONS] [PacketType] [Parameterss]\n", name );
  printf("Available Packet Types:\n" );


  printf("\n/*******************************************************************\n");
  printf("    SendCmd  - Message to set command in node\n");
  printf("*******************************************************************/\n");
  printf("SendCmd    cmd   \n");
  printf("\tcmd [uint8_t] -  \n");

  printf("\n/*******************************************************************\n");
  printf("    GetData  - \n");
  printf("*******************************************************************/\n");
  printf("GetData    \n");

  printf("\n/*******************************************************************\n");
  printf("    Data  - Message containing data from sensor\n");
  printf("*******************************************************************/\n");
  printf("Data    sensorA   sensorB   sensorName   \n");
  printf("\tsensorA [int16_t] - Value of Sensor A \n");
  printf("\tsensorB [int] - Value of Sensor B \n");
  printf("\tsensorName [char] - Name of sensor responding to message  \n");
  exit(1);
}


void sig_exit()
{
  printf("closing!\n");
  app_sample_end();
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