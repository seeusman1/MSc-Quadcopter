#include <stdio.h>
#include <string.h>
#include "../protocol.h"
#include "../statemanager/statemanager.h"
#include "../in4073.h"
#include "communication/communication.h"


void send_motors(){
	MotorMessage m_msg;
	m_msg.id = MOTOR;
	memcpy(m_msg.motor,motor,PAYLOAD_SIZE);
	
	send_message_unsafe((GenericMessage*) &m_msg);
}

void send_angles(){
	AngleMessage a_msg;
	a_msg.id = ANGLE;
	a_msg.phi = phi;
	a_msg.theta = theta;
	a_msg.psi = psi;
	
	send_message_unsafe((GenericMessage*) &a_msg);
}

void send_rates(){
	RateMessage r_msg;
	r_msg.id = RATE;
	r_msg.sp = sp;
	r_msg.sq = sq;
	r_msg.sr = sr;
	
	send_message_unsafe((GenericMessage*) &r_msg);
}

void send_stats(){
	StatMessage s_msg;
	s_msg.id = STAT;
	s_msg.temperature = temperature;
	s_msg.bat_volt = bat_volt;
	s_msg.mode = (uint8_t) get_current_state();
	
	send_message_unsafe((GenericMessage*) &s_msg);
}
#define TELE_FREQ 100000
//TODO: we can arrange each telemetry function to run on a different frequency
void send_telemetry() {
	static uint32_t old_t = 0;
	if ((get_time_us() - old_t) > TELE_FREQ)
	{	
		old_t = get_time_us();
		send_motors();
		send_angles();
		send_rates();
		send_stats();
	
	}
}
