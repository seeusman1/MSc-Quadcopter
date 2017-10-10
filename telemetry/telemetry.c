#include <stdio.h>
#include <string.h>
#include "../protocol.h"
#include "../statemanager/statemanager.h"
#include "../in4073.h"

void send_telemetry(){

	uint8_t i;
	
	MotorMessage m_msg;
	m_msg.id = MOTOR;
	uart_put(m_msg.id);
	memcpy(m_msg.motor,motor,4);
	for (i = 0; i < 4; i++)
	{
		uart_put(m_msg.motor[i]);
	}

	AngleMessage a_msg;
	a_msg.id = ANGLE;
	uart_put(a_msg.id);
	a_msg.phi = phi;
	uart_put(a_msg.phi);
	a_msg.theta = theta;
	uart_put(a_msg.theta);	
	a_msg.psi = psi;
	uart_put(a_msg.psi);

	RateMessage r_msg;
	r_msg.id = RATE;
	uart_put(r_msg.id);
	r_msg.sp = sp;
	uart_put(r_msg.sp);
	r_msg.sq = sq;
	uart_put(r_msg.sq);
	r_msg.sr = sr;
	uart_put(r_msg.sr);

	StatMessage s_msg;
	s_msg.id = STAT;
	uart_put(s_msg.id);
	s_msg.temperature = temperature;
	uart_put(s_msg.temperature);
	s_msg.bat_volt = bat_volt;
	uart_put(s_msg.bat_volt);
	s_msg.mode = get_current_state();
	uart_put(s_msg.mode);

}