/*------------------------------------------------------------------
 *  control.c -- here you can implement your control algorithm
 *		 and any motor clipping or whatever else
 *		 remember! motor input =  0-1000 : 125-250 us (OneShot125)
 *
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  July 2016
 *------------------------------------------------------------------
 */

#include "in4073.h"
#include "statemanager/statemanager.h"

#define MAX_SETPOINT 900
#define MIN_SETPOINT 200 // needs to check at what value rotor starts spinning
uint32_t P = 10;
uint32_t P1 = 1;
uint32_t P2 = 4; // P2 >= 4*P1

void update_motors(void)
{					

	motor[0] = ae[0];
	motor[1] = ae[1];
	motor[2] = ae[2];
	motor[3] = ae[3];
	
}

/*
 * Author: Rutger van den Berg
 * Control step for manual mode. 
 */
/* void manual() {
	// int16_t B = 1;
	// int16_t D = 1;
	int32_t Z = current_pose.lift;
	int32_t L = current_pose.roll;
	int32_t M = current_pose.pitch;
	int32_t N = current_pose.yaw;
	ae[0] = (-Z/4 + M/2 - -N/4 + INT16_MAX) /64;
	ae[1] = (-Z/4 -L/2 + N/4 + INT16_MAX) /64;
	ae[2] = (-Z/4 - M/2 - N/4 + INT16_MAX) /64;
	ae[3] = (-Z/4 + L/2 + N/4 + INT16_MAX) /64;
} */



 void speed_limit_check() {

	if (ae[0] < MIN_SETPOINT){
		ae[0] = MIN_SETPOINT;
	}else if (ae[0] > MAX_SETPOINT){
		ae[0] = MAX_SETPOINT;
	}

	if (ae[1] < MIN_SETPOINT){
		ae[1] = MIN_SETPOINT;
	}else if (ae[1] > MAX_SETPOINT){
		ae[1] = MAX_SETPOINT;
	}

	if (ae[2] < MIN_SETPOINT){
		ae[2] = MIN_SETPOINT;
	}else if (ae[2] > MAX_SETPOINT){
		ae[2] = MAX_SETPOINT;
	}

	if (ae[3] < MIN_SETPOINT){
		ae[3] = MIN_SETPOINT;
	}else if (ae[3] > MAX_SETPOINT){
		ae[3] = MAX_SETPOINT;
	}






	if ((ae[0] == MIN_SETPOINT) && (ae[2] > 600)){
		ae[2] = 600;
	}

	if ((ae[2] == MIN_SETPOINT) && (ae[0] > 600)){
		ae[0] = 600;
	}

	if ((ae[1] == MIN_SETPOINT) && (ae[3] > 600)){
		ae[3] = 600;
	}

	if ((ae[3] == MIN_SETPOINT) && (ae[1] > 600)){
		ae[1] = 600;
	}



	if ((ae[0] == MIN_SETPOINT) && (ae[2] == MIN_SETPOINT)){
		if (ae[1] > 600) ae[1]=600;
		if (ae[3] > 600) ae[3]=600;
	}

	if ((ae[1] == MIN_SETPOINT) && (ae[3] == MIN_SETPOINT)){
		if (ae[0] > 600) ae[0]=600;
		if (ae[2] > 600) ae[2]=600;
	}


		
}

 void manual() {
	
	int32_t Z;
	if (current_pose.lift < 0 ){
		Z = (current_pose.lift + INT16_MAX)/82;
	}else {
		Z = 400 + current_pose.lift/109; // at high speed, changing thrust becomes less sensitive
	}	


	int32_t L = (current_pose.roll)/64;
	int32_t M = (current_pose.pitch)/64;
	int32_t N = (current_pose.yaw)/64;
	
	if (Z >= MIN_SETPOINT){
	
		ae[0] = Z + M/4 - N/4;
		ae[1] = Z -L/4 + N/4;
		ae[2] = Z - M/4 - N/4;
		ae[3] = Z + L/4 + N/4;
		speed_limit_check();
	}else {
		ae[0] = 0;
		ae[1] = 0;
		ae[2] = 0;
		ae[3] = 0;
	}
}

void yaw_control() {

	int32_t Z;
	if (current_pose.lift < 0 ){
		Z = (current_pose.lift + INT16_MAX)/82;
	}else {
		Z = 400 + current_pose.lift/109; // at high speed, changing thrust becomes less sensitive
	}	


	int32_t L = (current_pose.roll)/64;
	int32_t M = (current_pose.pitch)/64;
	int32_t N = (current_pose.yaw)/64;
	
	int32_t N_new = P * (N-(sr/32)); // P controller	

	if (Z >= MIN_SETPOINT){
	
		ae[0] = Z + M/4 + N_new/4;
		ae[1] = Z -L/4 - N_new/4;
		ae[2] = Z - M/4 + N_new/4;
		ae[3] = Z + L/4 - N_new/4;
		speed_limit_check();
	}else {
		ae[0] = 0;
		ae[1] = 0;
		ae[2] = 0;
		ae[3] = 0;
	}

}



void full_control() {


	int32_t Z;
	if (current_pose.lift < 0 ){
		Z = (current_pose.lift + INT16_MAX)/82;
	}else {
		Z = 400 + current_pose.lift/109; // at high speed, changing thrust becomes less sensitive
	}	


	int32_t L = (current_pose.roll)/64;
	int32_t M = (current_pose.pitch)/64;
	int32_t N = (current_pose.yaw)/64;

	//uint8_t A1=1;
	//uint8_t A2=1;
	
	N = P * (N-(sr/32)); // P controller for Yaw
	L = P2 * (P1 * (L - phi/32) - sp/32); // Cascaded P controller for Roll 
	M = P2 * (P1 * (M - theta/32) + sq/32);//Cascaded P controller for Pitch
	

	if (Z >= MIN_SETPOINT){
	
		ae[0] = Z + M/4 + N/8;
		ae[1] = Z -L/4 - N/8;
		ae[2] = Z - M/4 + N/8;
		ae[3] = Z + L/4 - N/8;
		speed_limit_check();
	}else {
		ae[0] = 0;
		ae[1] = 0;
		ae[2] = 0;
		ae[3] = 0;
	}

}




int decrement(int i) {
	int j = i - 5;
	if (j < 0) {
		j = 0;
	}
	return j;
}

void panic() {
	ae[0] = decrement(ae[0]);
	ae[1] = decrement(ae[1]);
	ae[2] = decrement(ae[2]);
	ae[3] = decrement(ae[3]);
}

void safe() {
	ae[0] = 0;//(current_pose.lift/64) + 512;
	ae[1] = 0;//(current_pose.lift/64) + 512;
	ae[2] = 0;//(current_pose.lift/64) + 512;
	ae[3] = 0;//(current_pose.lift/64) + 512;
}

void run_filters_and_control()
{	
	switch (get_current_state())
	{
		case SAFE:
			safe();
			break;
		case PANIC:
			panic();
			break;
		case MANUAL:
			manual();
		case CALIBRATION:
			break;
		case YAWCONTROL:
			yaw_control();
			break;
		case FULLCONTROL:
			full_control();
			break;
		default:
			printf("Control encountered unexpected mode %u\n", get_current_state());
			break;
	}
	// fancy stuff here
	// control loops and/or filters

	// ae[0] = xxx, ae[1] = yyy etc etc
	update_motors();
}
