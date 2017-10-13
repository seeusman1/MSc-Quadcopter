#include <stdio.h>
#include <string.h>
#include "../protocol.h"
#include "../statemanager/statemanager.h"
#include "../in4073.h"


void send_motors(){

	uint8_t i;
	MotorMessage m_msg;
	m_msg.id = MOTOR;
	memcpy(m_msg.motor,motor,8);
	char* ptr_data = (char*) &m_msg.motor;
	
	uart_put(m_msg.id);
	for (i = 0; i < PAYLOAD_SIZE; i++)
	{
		uart_put(*ptr_data++);
	}
}

void send_angles(){
	
	uint8_t i;
	AngleMessage a_msg;
	a_msg.id = ANGLE;
	a_msg.phi = phi;
	a_msg.theta = theta;
	a_msg.psi = psi;
	
	char* ptr_data = (char*) &a_msg.phi;


	uart_put(a_msg.id);
	for (i = 0; i < PAYLOAD_SIZE; i++)
	{
		uart_put(*ptr_data++);
	}

}

void send_rates(){

	uint8_t i;
	RateMessage r_msg;
	r_msg.id = RATE;
	r_msg.sp = sp;
	r_msg.sq = sq;
	r_msg.sr = sr;
	
	char* ptr_data = (char*) &r_msg.sp;
	
	uart_put(r_msg.id);
	for (i = 0; i < PAYLOAD_SIZE; i++)
	{
		uart_put(*ptr_data++);
	}

}

void send_stats(){

	
	uint8_t i;
	StatMessage s_msg;
	s_msg.id = STAT;
	s_msg.temperature = temperature;
	s_msg.bat_volt = bat_volt;
	s_msg.mode =(uint8_t) get_current_state();
	
	char* ptr_data = (char*) &s_msg.temperature;
	
	uart_put(s_msg.id);
	for (i = 0; i < PAYLOAD_SIZE; i++)
	{
		uart_put(*ptr_data++);
	}

}

#ifdef PROFILING
void send_profiling(){
	
	uint8_t i;
	ProfMessage p_msg;
	p_msg.id = PROF;
	p_msg.cont_time = cont_time;
	p_msg.tele_time = tele_time;

	char* ptr_data = (char*) &p_msg.cont_time;
	
	uart_put(p_msg.id);
	for (i = 0; i < PAYLOAD_SIZE; i++)
	{
		uart_put(*ptr_data++);
	}

}
#endif

#define TELE_FREQ 100000
//TODO: we can arrange each telemetry function to run on a different frequency
void send_telemetry(){

	static uint32_t old_t = 0;

	if ((get_time_us() - old_t) > TELE_FREQ)
	{	
		old_t = get_time_us();
		send_motors();
		send_angles();
		send_rates();
		send_stats();
		#ifdef PROFILING
		send_profiling();
		#endif
	
	}
	

}