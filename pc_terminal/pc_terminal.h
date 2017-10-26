
#ifndef __PC_TERMINAL_H
#define __PC_TERMINAL_H

#define JOYSTICK_PRESENT
#define PROFILING
#include "interface/interface.h"
#include "messaging/messaging.h"
#include "../protocol.h"
#include <stdio.h>
//#include "../crc/crc.h"

queue receive_queue;
//Joystic Globals
int axis[6];
int button[12]; 
int fd;

//terminal termination
int term;

//open file
FILE *f;

//character variable
char	c;

struct termios 	savetty;

//serial
int serial_device;
int fd_RS232;

#endif //__PC_TERMINAL_H