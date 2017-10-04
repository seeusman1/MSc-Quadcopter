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
#include <pthread.h>
#include "pc_terminal.h"

#include <stdint.h>
#include <assert.h>
/**********************Messaging QUEUE*************************/
#define QUEUE_SIZE 256
typedef struct {
	uint8_t Data[QUEUE_SIZE];
	uint16_t first,last;
  	uint16_t count; 
} queue;
void init_queue(queue *q);
void enqueue(queue *q, char x);
char dequeue(queue *q);

void init_queue(queue *q){
	
	q->first = 0;
	q->last = QUEUE_SIZE - 1;
	q->count = 0;
}

void enqueue(queue *q,char x){
	if (q->count >= QUEUE_SIZE) {
		printf("PC queue full");
		return;
	}
	q->last = (q->last + 1) % QUEUE_SIZE;
	q->Data[ q->last ] = x;
	q->count += 1;
}

char dequeue(queue *q){
	if (q->count == 0) {
		printf("PC is empty!");
		return 0;
	}
	char x = q->Data[ q->first ];
	q->first = (q->first + 1) % QUEUE_SIZE;
	q->count -= 1;
	return x;
}

queue receive_queue;


void handle_message()
{
uint8_t msg[MESSAGE_SIZE];
	PrintMessage *msp = (PrintMessage*) &msg[0];
	if (receive_queue.count >= MESSAGE_SIZE){

		for(uint8_t i=0; i < MESSAGE_SIZE; i++){
			msg[i] = dequeue(&receive_queue);
			
		}
		//printf("check!!\n");
		switch (msg[0])
		{
			case PRINT:
				//printf("{\n");
				printf("%s",msp->data);
				//printf("}\n");
				break;
		}
	}
}

void incoming_msg_check(){ 
	int 		result;
	unsigned char 	c;

	result = read(fd_RS232, &c, 1);

	if (result == 0)
		return;

	else
	{
		assert(result == 1);
		enqueue(&receive_queue,c);
		
	}

	if (receive_queue.count>=MESSAGE_SIZE)
	{
		handle_message();
	}

}


/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */
int main(int argc, char **argv)
{
	
	char	ck;

	term_puts("\nTerminal program - Embedded Real-Time Systems\n");

	term_initio();
	rs232_open();
	init_queue(&receive_queue);
	/*Joystic initialize
	*/


	 pthread_t j_thrd,k_thrd;

	term_puts("Type ^C to exit\n");

	/* discard any incoming text
	 */
	while ((c = rs232_getchar_nb()) != -1)
		fputc(c,stderr);
	


	if(pthread_create(&k_thrd, NULL, key_thread, NULL)) {

		fprintf(stderr, "Error creating thread\n");
		return 1;

	}

	#ifdef JOYSTICK_PRESENT
	fd = joy_init();

	if(pthread_create(&j_thrd, NULL, joy_thread, NULL)) {

		fprintf(stderr, "Error creating thread\n");
		return 1;

	}	
	#endif
	/* send & receive
	 */
	while(!term)	
	{	

		

		// if ((ck = rs232_getchar_nb()) != -1)
		// 	term_putchar(ck);

		incoming_msg_check();

	}
	if(pthread_join(k_thrd, NULL)) {

		fprintf(stderr, "Error joining thread\n");
		return 2;

	}
	#ifdef JOYSTICK_PRESENT
	if(pthread_join(j_thrd, NULL)) {

		fprintf(stderr, "Error joining thread\n");
		return 2;

	}
	#endif
	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");

	return 0;
}

