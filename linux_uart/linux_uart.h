/**
  *@file linux_uart.h
  *@brief linux serial module
  *@author Jason Berger
  *@date 03/22/2019
  */

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "stdint.h"
#include <stdbool.h>




/**
  *@brief open uart port
  *@param fd ptr to int to store uart handle
  *@param device name of device to connect to i.e. "/dev/ttyS16"
  *@param baudrate baudrate for connection [2400 ,4800, 9600, 19200, 38400, 57600, 115200]
  *@return true if successful
  *@return false if there was a problem
  */
bool uart_open(int* fd,const char * device, int baudRate);

/**
  *@brief closes the uart port
  *@param fd uart handle
  */
void uart_close(int fd);

/**
  *@brief read from uart port
  *@param fd uart handle
  *@param data ptr to store data read
  *@param number of bytes to read
  *@return number of bytes read
  */
int uart_read(int fd, uint8_t* data, int len);

/**
  *@brief write to uart port
  *@param fd uart handle
  *@param data ptr to data to write
  *@param number of bytes to write
  *@return number of bytes written
  */
int uart_write(int fd, uint8_t* data, int len);
