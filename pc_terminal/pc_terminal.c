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
#include "../protocol.h"
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

// int rs232_sendMsg(queue buf){
// 	int current = buf.first;
// 	int sts;
// 	int q_count = buf.count;

// 	for (current; current < buf.last; ++current)
// 	{	
// 		sts=rs232_putchar(Data[current]);
// 		q_count--;
		
// 		if (sts != 1){
// 			printf("Failed:rs232_sendMsg\n");
// 		}

// 	}
// 	if (q_count != 0){
// 		printf("Queue not emptied\n");
// 	}
// }
/* 
 * Author: Dimitris 
 * 
 */
void rs232_createMsg_mode(char c, ModeMessage msg){
	
	switch(c) {
		case '0':
		//MessageId = 1;
		msg.mode = 0;
		case '1':
		//MessageId = 1;
		msg.mode = 1;
		case '2':
		//MessageId = 1;
		msg.mode = 2;
		case '3':
		//MessageId = 1;
		msg.mode = 3;
		case '4':
		//MessageId = 1;
		msg.mode = 4;
		case '5':
		///MessageId = 1;
		msg.mode = 5;
		case '6':
		//MessageId = 1;
		msg.mode = 6;
		case '7':
		//MessageId = 1;
		msg.mode = 7;
		case '8':
		//MessageId = 1;
		msg.mode = 8;

		default :
		printf("Invalid Mode!,defaults in 0 mode\n");
		//MessageId = 1;
		msg.mode = 0;
	}
}

// void rs232_createMsg_joystick (unsigned char axes, JoystickMessage msg){

// 	//Dummy function to be used as Joystick Messaging 
// 	//msg.MessageId = 2;

// 	//Derive lift,roll,pitch,yaw
// 	msg.pose.lift = 1;

// 	msg.pose.roll = 1;

// 	msg.pose.pitch = 1;

// 	msg.pose.yaw = 1;

// }

/* Joystick area 

*/


// int axis[6];
// int button[12]; 

// unsigned int  mon_time_ms(void)
// {
//         unsigned int    ms;
//         struct timeval  tv;
//         struct timezone tz;

//         gettimeofday(&tv, &tz);
//         ms = 1000 * (tv.tv_sec % 65); // 65 sec wrap around
//         ms = ms + tv.tv_usec / 1000;
//         return ms;
// }

// int joy_init(){
// 	int fd;

// 	//Initialize Joystick
// 	if ((fd = open(JS_DEV, O_RDONLY)) < 0) {
// 		perror("jstest");
// 		exit(1);
// 	}

	
// 	/* non-blocking mode
// 	 */
// 	fcntl(fd, F_SETFL, O_NONBLOCK);

// 	return fd;
// }

/*
 * Author Rutger van den Berg
 * Writes 9 bytes from the provided pointer to UART.
 * Assumes that 9 bytes containing a single message can be found at the specified location. 
 */
void send_message(char *msg) {
	int sts;

	for (uint8_t i = 0; i < MESSAGE_SIZE; i++)
	{	
		sts=rs232_putchar(msg[i]);
		
		if (sts != 1){
			printf("Failed:rs232_sendMsg\n");
		}

	}
}


/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */
int main(int argc, char **argv)
{
	// struct js_event js;
	
	char	c;

	term_puts("\nTerminal program - Embedded Real-Time Systems\n");

	term_initio();
	rs232_open();



	term_puts("Type ^C to exit\n");

	/* discard any incoming text
	 */
	while ((c = rs232_getchar_nb()) != -1)
		fputc(c,stderr);

	/* send & receive
	 */
	for (;;)
	{
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

