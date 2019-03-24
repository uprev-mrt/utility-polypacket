/**
  *@file linux_uart.c
  *@brief linux serial module
  *@author Jason Berger
  *@date 03/22/2019
  */

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>


#include "linux_uart.h"

void uart_config(int fd, int baudRate)
{

  struct termios SerialPortSettings;
  int baudSymbol;

  switch(baudRate)
  {
    case 2400 :
      baudSymbol = B2400;
      break;
    case 4800 :
      baudSymbol = B4800;
      break;
    case 9600 :
      baudSymbol = B9600;
      break;
    case 19200 :
      baudSymbol = B19200;
      break;
    case 38400 :
      baudSymbol = B38400;
      break;
    case 57600 :
      baudSymbol = B57600;
      break;
    case 115200 :
      baudSymbol = B115200;
      break;
    default :
      baudSymbol = B9600;
      break;
  }

  tcgetattr(fd, &SerialPortSettings);

  cfsetispeed(&SerialPortSettings,baudSymbol);
  cfsetospeed(&SerialPortSettings,baudSymbol);

  SerialPortSettings.c_lflag =0;

  SerialPortSettings.c_cflag &= ~PARENB;   // No Parity
  SerialPortSettings.c_cflag &= ~CSTOPB; //Stop bits = 1
  SerialPortSettings.c_cflag &= ~CSIZE; /* Clears the Mask       */
  SerialPortSettings.c_cflag |=  CS8;   /* Set the data bits = 8 */

  SerialPortSettings.c_iflag&=~(IXON | IXOFF | IXANY | ICRNL | INLCR | IGNCR ); /*  */
  SerialPortSettings.c_oflag&=~(OPOST | ONLCR|OCRNL);   /*  */

  SerialPortSettings.c_cflag |= CREAD | CLOCAL;//enable receiver

  /* Setting Time outs */
  SerialPortSettings.c_cc[VMIN]  = 0;  /* This and VTIME make this a timed read */
  SerialPortSettings.c_cc[VTIME] = 1;  /* Wait up to .1 sec for data to arrive */

  tcsetattr(fd,TCSANOW,&SerialPortSettings);
}

bool uart_open(int* fd, const char * device, int baudRate)
{
    *fd = open(device , O_RDWR | O_NOCTTY);
    if(*fd == 1)
    {
      return false;
    }

    uart_config(*fd,baudRate);
    tcflush(*fd,TCOFLUSH);
    return true;
}

void uart_close(int fd)
{
  close(fd);
}

int uart_write(int fd, uint8_t* data, int len)
{
  int outCount = write(fd,data, len );
  return outCount;
}

int uart_read(int fd, uint8_t* data, int len)
{
  int inCount = read(fd, data, len);
  return inCount;
}
