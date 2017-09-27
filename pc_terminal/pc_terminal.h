
#ifndef __PC_TERMINAL_H
#define __PC_TERMINAL_H

#define JOYSTICK_PRESENT

#include "interface/interface.h"
#include "../protocol.h"
//#include "../crc/crc.h"

//Joystic Globals
int axis[6];
int button[12]; 
int fd;

//terminal termination
int 	term = 0;

//character variable
char	c;

struct termios 	savetty;

//serial
int serial_device = 0;
int fd_RS232;

#endif //__PC_TERMINAL_H