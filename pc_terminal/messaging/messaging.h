
#ifndef __MESSAGING_H
#define __MESSAGING_H

//#define JOYSTICK_PRESENT


#define QUEUE_SIZE 256

typedef struct {
	uint8_t Data[QUEUE_SIZE];
	uint16_t first,last;
  	uint16_t count; 
} queue;

queue receive_queue;

void init_queue(queue *q);
void enqueue(queue *q, char x);
char dequeue(queue *q);


int fd_RS232;

void handle_message();
void incoming_msg_check();

#endif //__MESSAGING_H
