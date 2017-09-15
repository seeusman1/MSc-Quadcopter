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
void manual() {
	int16_t B = 1;
	int16_t D = 1;
	int32_t Z = current_pose.lift;
	int32_t L = current_pose.roll;
	int32_t M = current_pose.pitch;
	int32_t N = current_pose.yaw;
	ae[0] = (-Z/4 + M/2 - -N/4 + INT16_MAX) /64;
	ae[1] = (-Z/4 -L/2 + N/4 + INT16_MAX) /64;
	ae[2] = (-Z/4 - M/2 - N/4 + INT16_MAX) /64;
	ae[3] = (-Z/4 + L/2 + N/4 + INT16_MAX) /64;
}



void run_filters_and_control()
{
	switch (get_current_state())
	{
		case SAFE:
			break;
		case PANIC:
			break;
		case MANUAL:
			manual();
		case CALIBRATION:
			break;
		case YAWCONTROL:
			break;
		case FULLCONTROL:
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
