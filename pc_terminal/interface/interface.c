#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>

#include <signal.h>
#include <pthread.h>
#include <errno.h>


#include "../joystick.h"
#include "../../protocol.h"
#include "../../crc/crc.h"

#include "interface.h"

//#define JOYSTICK_PRESENT

/*------------------------------------------------------------
 * Serial I/O
 * 8 bits, 1 stopbit, no parity,
 * 115,200 baud
 *------------------------------------------------------------
 */

int serial_device;
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





//Joystic Globals
int axis[6];
int button[12]; 
int fd;
int JS_FLAGS = 0;

//terminal termination
int 	term;

//character variable
char	c;

struct termios 	savetty;




/*------------------------------------------------------------
 * console I/O
 *------------------------------------------------------------
 */


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
       static unsigned char line [4];

        if (read(0,line,3)) {
        	if (line[1] == '[')
        	{	
        		line[1] = 0;
        		switch (line[2]){
			        //ARROW KEYS-only active when arrow key is pressed
					case 'A':
					//UP
					return 28;
					case 'B':
					//DOWN
					return 29;
					case 'C':
					//RIGHT
					return 30;
					case 'D':
					//LEFT
					return 31;
					default:
					return 0;
        		}
        		
        	}
        	return (int) line[0];

        }
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
 * Messaging
 *------------------------------------------------------------
 */

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



/*------------------------------------------------------------
 * Joystic Infastructure
 *------------------------------------------------------------
 */


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

/*
* Author D.Patoukas
*
*
*/
int joy_read(int * axis, int * button, int fd){
	struct js_event js;	

	if(read(fd, &js, sizeof(struct js_event)) ==  sizeof(struct js_event)) {
		JS_FLAGS |= js.type;
		switch(js.type) {
			case JS_EVENT_BUTTON:
				button[js.number] = js.value;
				return 1;
			case JS_EVENT_AXIS:
				axis[js.number] = js.value;
				return 1;
			case JS_EVENT_INIT:
				printf("Unexpected JS_EVENT_INIT read...\n");
				 return 0;
			default:
				return 0;
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

	pose.lift = -axis[3];
	pose.roll = axis[0];
	pose.pitch = axis[1];
	pose.yaw = axis[2];

	return pose;

}



/*
* Author: Rutger van den Berg
* 
* Timer handler for the joystick. Reads all joystick events, 
* and then sends the most recent value for the axes or buttons
* if they were read.
*/
JoystickMessage current_JM;
JoystickPose currentPose;
void joy_handler(union sigval val) {
	JS_FLAGS = 0;
	while (joy_read(axis,button,fd)){

	}
	if (JS_FLAGS & JS_EVENT_BUTTON){
		ModeMessage msg;
		msg.id = MODE;
		msg.mode = (char) 4;
		send_message((char*) &msg);
	} 
	
	//always send axes position so we can use this as heartbeat message.
	currentPose = calculate_pose(axis,button);
	current_JM = rs232_createMsg_joystick(axis,currentPose);
	send_message((char*) &current_JM);

}
/*
 * Author: D.Patoukas
 * Reads the inputed character and returns a ModeMessage to be sent 
 *  
 */

ModeMessage rs232_createMsg_mode(char c){
	
	ModeMessage msg;
    
	msg.id = MODE;
	switch(c){
		//ESC Button
		case 27:
		msg.mode = 27;
		break;
		case 'q':
		case 'a':
		case 'w':
		case 's':
		case 'e':
		case 'd':
		case 'r':
		case 'f':
		case 'u':
		case 'j':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		msg.mode = c;
		break;
		//ARROW KEYS-only active when arrow key is pressed
		case 28:
		//UP
		msg.mode = 28;
		break;
		case 29:
		//DOWN
		msg.mode = 29;
		break;
		case 30:
		//RIGHT
		msg.mode = 30;
		break;
		case 31:
		//LEFT
		msg.mode = 31;
		break;
		default :
		printf("Invalid Mode!,defaults in 0 mode\n");
		msg.mode = 0;
}
	return msg;
}
		
		
		
		
/*
* Author D.Patoukas
*
*
*/
void *key_thread(){

	ModeMessage current_MM;
	while(!term){
	if ((c = term_getchar_nb()) != -1){
		// rs232_putchar(c);

		if (c != 0 ){
			current_MM = rs232_createMsg_mode(c);
			send_message((char*) &current_MM);
		}

	}
	}
	return NULL;

}
