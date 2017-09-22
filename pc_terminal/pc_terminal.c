/*------------------------------------------------------------
 * Simple pc terminal in C
 *
 * Arjan J.C. van Gemund (+ mods by Ioannis Protonotarios)
 *
 * read more: http://mirror.datenwolf.net/serial/
 *------------------------------------------------------------
 */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

#include "../protocol.h"
#include "../crc/crc.h"
#include "joystick.h"


#define JOYSTICK_PRESENT

/*------------------------------------------------------------
 * console I/O
 *------------------------------------------------------------
 */
struct termios 	savetty;

void	term_initio()
{
	struct termios tty;

	tcgetattr(0, &savetty);
	tcgetattr(0, &tty);

	tty.c_lflag &= ~(ECHO|ECHONL|ICANON|IEXTEN);
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 0;

	tcsetattr(0, TCSADRAIN, &tty);
}

void	term_exitio()
{
	tcsetattr(0, TCSADRAIN, &savetty);
}

void	term_puts(char *s)
{
	fprintf(stderr,"%s",s);
}

void	term_putchar(char c)
{
	putc(c,stderr);
}

int	term_getchar_nb()
{
        static unsigned char 	line [2];

        if (read(0,line,1)) // note: destructive read
        		return (int) line[0];

        return -1;
}

int	term_getchar()
{
        int    c;

        while ((c = term_getchar_nb()) == -1)
                ;
        return c;
}

/*------------------------------------------------------------
 * Serial I/O
 * 8 bits, 1 stopbit, no parity,
 * 115,200 baud
 *------------------------------------------------------------
 */
#include <termios.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

int serial_device = 0;
int fd_RS232;

void rs232_open(void)
{
  	char 		*name;
  	int 		result;
  	struct termios	tty;

       	fd_RS232 = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);  // Hardcode your serial port here, or request it as an argument at runtime

	assert(fd_RS232>=0);

  	result = isatty(fd_RS232);
  	assert(result == 1);

  	name = ttyname(fd_RS232);
  	assert(name != 0);

  	result = tcgetattr(fd_RS232, &tty);
	assert(result == 0);

	tty.c_iflag = IGNBRK; /* ignore break condition */
	tty.c_oflag = 0;
	tty.c_lflag = 0;

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; /* 8 bits-per-character */
	tty.c_cflag |= CLOCAL | CREAD; /* Ignore model status + read input */

	cfsetospeed(&tty, B115200);
	cfsetispeed(&tty, B115200);

	tty.c_cc[VMIN]  = 0;
	tty.c_cc[VTIME] = 1; // added timeout

	tty.c_iflag &= ~(IXON|IXOFF|IXANY);

	result = tcsetattr (fd_RS232, TCSANOW, &tty); /* non-canonical */

	tcflush(fd_RS232, TCIOFLUSH); /* flush I/O buffer */
}


void 	rs232_close(void)
{
  	int 	result;

  	result = close(fd_RS232);
  	assert (result==0);
}


int	rs232_getchar_nb()
{
	int 		result;
	unsigned char 	c;

	result = read(fd_RS232, &c, 1);

	if (result == 0)
		return -1;

	else
	{
		assert(result == 1);
		return (int) c;
	}
}


int 	rs232_getchar()
{
	int 	c;

	while ((c = rs232_getchar_nb()) == -1)
		;
	return c;
}


int 	rs232_putchar(char c)
{
	int result;

	do {
		result = (int) write(fd_RS232, &c, 1);
	} while (result == 0);

	assert(result == 1);
	return result;
}

/*
 * Author Rutger van den Berg
 * Writes 9 bytes from the provided pointer to UART.
 * Assumes that 9 bytes containing a single message can be found at the specified location. 
 */
void send_message(char *msg) {
	CRCMessage message = make_packet(msg);
	int sts;
	uint8_t i;
	for (i = 0; i < CRC_MESSAGE_SIZE; i++)
	{	
		sts=rs232_putchar(((char*) (&message))[i]);
		
		if (sts != 1){
			printf("Failed:rs232_sendMsg\n");
		}

	}
}



/********************Joystic Infastructure*********************/
#define JS_DEV	"/dev/input/js1"

int axis[6];
int button[12]; 


int joy_init(){
	int fd;

	//Initialize Joystick
	if ((fd = open(JS_DEV, O_RDONLY)) < 0) {
		perror("jstest");
		exit(1);
	}

	
	/* non-blocking mode
	 */
	fcntl(fd, F_SETFL, O_NONBLOCK);

	return fd;
}

int joy_read(int * axis, int * button, int fd){
	struct js_event js;

	while(read(fd, &js, sizeof(struct js_event)) == sizeof(struct js_event)){
		switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				button[js.number] = js.value;
				return 1;
			case JS_EVENT_AXIS:
				axis[js.number] = js.value;
				return 1;
		}
	}
	return 0;
}
/*
* Author D.Patoukas
*
*
*/
JoystickMessage rs232_createMsg_joystick (int axis[], JoystickPose pose){

	JoystickMessage msg;

	msg.id = JOYSTICK;
	msg.pose = pose;

	return msg;

}

/*
* Author D.Patoukas
*
*
*/
JoystickPose calculate_pose(int axis[], int button[]){
	JoystickPose pose;

	pose.lift = axis[3];
	pose.roll = axis[0];
	pose.pitch = axis[1];
	pose.yaw = axis[2];

	return pose;

}





/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */
int main(int argc, char **argv)
{
	
	char	c;

	term_puts("\nTerminal program - Embedded Real-Time Systems\n");

	term_initio();
	rs232_open();

	/*Joystic initialize
	*/
	#ifdef JOYSTICK_PRESENT
	int fd = joy_init();

	JoystickPose currentPose;
	JoystickMessage current_JM;
	#endif

	term_puts("Type ^C to exit\n");

	/* discard any incoming text
	 */
	while ((c = rs232_getchar_nb()) != -1)
		fputc(c,stderr);

	/* send & receive
	 */
	for (;;)
	{	

		#ifdef JOYSTICK_PRESENT
		if (joy_read(axis,button,fd)){
			currentPose = calculate_pose(axis,button);
			current_JM = rs232_createMsg_joystick(axis,currentPose);
			send_message((char*) &current_JM);
		}

		if (button[0])
			break;
		#endif

		if ((c = term_getchar_nb()) != -1){

			// rs232_putchar(c);
			ModeMessage msg;
			msg.id = MODE;
			msg.mode = c;
			send_message((char*) &msg);

		}

		if ((c = rs232_getchar_nb()) != -1)
			term_putchar(c);

	}

	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");

	return 0;
}

