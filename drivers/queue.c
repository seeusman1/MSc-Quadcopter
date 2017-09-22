/*------------------------------------------------------------------
 *  queue.c -- some queue implementation stolen from the interwebs
 *
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  July 2016
 *------------------------------------------------------------------
 */

#include "in4073.h"

void init_queue(queue *q){
	
	q->first = 0;
	q->last = QUEUE_SIZE - 1;
	q->count = 0;
}

void enqueue(queue *q,char x){
	if (q->count >= QUEUE_SIZE) {
		printf("Queue is full!");
		return;
	}
	q->last = (q->last + 1) % QUEUE_SIZE;
	q->Data[ q->last ] = x;
	q->count += 1;
}

char dequeue(queue *q){
	if (q->count == 0) {
		printf("Queue is empty!\n");
		return 0;
	}
	char x = q->Data[ q->first ];
	q->first = (q->first + 1) % QUEUE_SIZE;
	q->count -= 1;
	return x;
}
/*
 * Author: Rutger van den Berg
 * retrieve at the character in a certain position without removing it from the queue. 
 */
char peek(queue *q, uint16_t position) {
	if(q->count <= position) {
		printf("Attempting to peek nonexistant character.\n");
		return 0;
	}
	char x = q->Data[(q->first + position) % QUEUE_SIZE];

	return x;
}