#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>

#include "messaging.h"
#include "../../protocol.h"
#include "../../logging/logging.h"
#include "../pc_terminal.h"

/**********************Messaging QUEUE*************************/


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

/*
* Author D.Patoukas
* Handles the messages of type LoggedData
* 
*/

//number of messages per LOG
uint8_t rmn = sizeof(LoggedData)/PAYLOAD_SIZE;

void handle_log(LogMessage* m) {

	uint8_t rcvd = sizeof(LoggedData)/PAYLOAD_SIZE - rmn;

	if (rmn == sizeof(LoggedData)/PAYLOAD_SIZE)
	{
	  loggdata = (LoggedData*) malloc(sizeof(LoggedData));
	  ptr_data = (uint8_t*) &(loggdata->padding);
	}

	memcpy (ptr_data+(rcvd*PAYLOAD_SIZE), &(m->data), PAYLOAD_SIZE);
	rmn--;
	
	//Once all log messages are received print a log message
	if (rmn == 0)
	{
		fprintf(f,"%d\t %6d\t%6d\t%6d\t", -
			loggdata->mode,loggdata->sp,loggdata->sq,loggdata->sr);
		fprintf(f, "%6d %6d %6d", 
			loggdata->phi,loggdata->theta,loggdata->psi);
		fprintf(f, "%4d\t%4d\t%4d\t %4d\t %4d\t %4d\t %4d\t \t\t%4d ",
			loggdata->ae[0],loggdata->ae[1],loggdata->ae[2],loggdata->ae[3],
			loggdata->motor[0],loggdata->motor[1],loggdata->motor[2],loggdata->motor[3]);
		fprintf(f, "\t%4d\t %4d\t %6d \n",
			loggdata->bat_volt,loggdata->temperature,loggdata->pressure);
		
		rmn = sizeof(LoggedData)/PAYLOAD_SIZE;;
	}
}

/*
* Author D.Patoukas
* Handles the messages of all types pc-side
* 
*/
void handle_message()
{

	uint8_t msg[MESSAGE_SIZE];
	PrintMessage* msp = (PrintMessage*) &msg[0];
	LogMessage* m =(LogMessage*) &msg[0];
	
	
	if (receive_queue.count >= MESSAGE_SIZE){

		for(uint8_t i=0; i < MESSAGE_SIZE; i++){
			msg[i] = dequeue(&receive_queue);
			
		}
		switch (msg[0])
		{
			case PRINT:
				printf("%s",msp->data);
				break;
			case LOG:
				handle_log(m);
				
				break;
		}
	}
}

/*
* Author D.Patoukas
* Message checking pc side
*
*/
int incoming_msg_check(){ 
	int 		result;
	unsigned char 	c;

	result = read(fd_RS232, &c, 1);

	if (result == 0)
		return 0;

	else
	{
		assert(result == 1);
		enqueue(&receive_queue,c);
		
	}

	if (receive_queue.count>=MESSAGE_SIZE)
	{
		handle_message();
	}
	return 1;

}