
#ifndef __MESSAGING_H
#define __MESSAGING_H


#include "../../logging/logging.h"


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

LoggedData* loggdata;
uint8_t* ptr_data;

int pr_motor[4];
int pr_batt,pr_temp,pr_mode;
int pr_psi,pr_theta,pr_phi;
int pr_sp,pr_sq,pr_sr; 

void handle_message();
int incoming_msg_check();

#endif //__MESSAGING_H
