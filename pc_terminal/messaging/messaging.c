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
	--rmn;
	
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
int s_flag = 0;
int m_flag = 0;
int a_flag = 0;
int r_flag = 0;
int p_flag = 0;

void handle_message()
{

	uint8_t msg[MESSAGE_SIZE];
	LogMessage*   l_msg = (LogMessage*)   &msg[0];
	PrintMessage* p_msg = (PrintMessage*) &msg[0];
	MotorMessage* m_msg = (MotorMessage*) &msg[0];			
	AngleMessage* a_msg = (AngleMessage*) &msg[0];
	RateMessage*  r_msg = (RateMessage*)  &msg[0];
	StatMessage*  s_msg = (StatMessage*)  &msg[0];
	ProfMessage*  pr_msg = (ProfMessage*)  &msg[0];
	
	if (receive_queue.count >= MESSAGE_SIZE){

		for(uint8_t i=0; i < MESSAGE_SIZE; i++){
			msg[i] = dequeue(&receive_queue);
			
		}
		switch (msg[0])
		{
			case PRINT:
				printf("%s",p_msg->data);
				break;
			case LOG:
				handle_log(l_msg);
				break;
			case MOTOR:
				//printf("M:%3d %3d %3d %3d",m_msg->motor[0],m_msg->motor[1],m_msg->motor[2],m_msg->motor[3]);
				m_flag = 1;
				pr_motor[0] = m_msg->motor[0];
				pr_motor[1] = m_msg->motor[1];
				pr_motor[2] = m_msg->motor[2];
				pr_motor[3] = m_msg->motor[3];
				break;
			case ANGLE:
				//printf("|phi %6d,theta %6d,psi %6d|",a_msg->phi,a_msg->theta,a_msg->psi);
				a_flag = 1;
				pr_psi = a_msg->psi;
				pr_theta = a_msg->theta;
				pr_phi = a_msg->phi;
				break;
			case RATE:
				//printf("|sq %6d,sq %6d,sr %6d|",r_msg->sp,r_msg->sq,r_msg->sr);
				r_flag = 1;
				pr_sp = r_msg->sp;
				pr_sq = r_msg->sq;
				pr_sr = r_msg->sr;
				break;
			case STAT:
				//printf("|mode %1d|temp %4d|batt %4d\n",s_msg->mode,s_msg->temperature,s_msg->bat_volt);
				s_flag = 1;
				pr_temp = s_msg->temperature;
				pr_batt = s_msg->bat_volt;
				pr_mode = s_msg->mode;
				break;
			#ifdef PROFILING
			case PROF:
				p_flag = 1;
				pr_cont_time = pr_msg->cont_time;
				pr_tele_time = pr_msg->tele_time;
				break;
			#else
				p_flag = 1;
			#endif
		}
		if (r_flag && m_flag && s_flag && a_flag && !term && p_flag)
		{
			#ifdef PROFILING
			printf("Control:%4d,Tele:%4d|",pr_cont_time,pr_tele_time);
			#endif
			printf("%3d %3d %3d %3d | ",pr_motor[0],pr_motor[1],pr_motor[2],pr_motor[3]);
			printf("%6d %6d %6d | ", pr_phi, pr_theta, pr_psi);
			printf("%6d %6d %6d | ", pr_sp, pr_sq, pr_sr);
			printf("%4d | %4d | %1d\n", pr_batt, pr_temp, pr_mode);
			
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