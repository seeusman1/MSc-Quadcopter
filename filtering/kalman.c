#include "kalman.h"
#include "../in4073.h"
constexpr ATTITUDE_BITS_PER_DEGREE = 32768/90; 	// -2^15 to +2^15 for -90 degrees to +90 degrees
constexpr P2DEGREE_DIVIDER = 16.4/1000; 		//16.4 lsb/degree/sec, and period is measured in ms
constexpr P2PHI = ATTITUDE_BITS_PER_DEGREE/P2DEGREE_DIVIDER; //we want to go from degrees/second to degrees, so multiply P by the period


#define C1 100
#define C2 1000000

extern int16_t p, q;			// Angular rate output. filtered.
extern int16_t phi, theta; 		// Attitude output. filtered.
extern int16_t sp, sq; 			// Angular rate input. updated by Gyro
extern int16_t sphi, stheta;	// Attitude input. updated by ?

uint32_t previous_sample_time = 0;
uint32_t period_ms = 0;
int16_t p_b, q_b;				//Estimated angular rate bias

uint16_t get_angle_change() {
	int32_t p_temp = p * 1024;


}





void kalman_filter() {
	/*
	 * update angular rate estimation. 
	 * 
	 * Measured value minus calculated bias
	 */
	p = sp - p_b;
	q = sq - q_b;

	/*
	 * first estimate of attitude.
	 *
	 * Uses the previous attitude and the 
	 * estimated rate to estimate the new attitude.
	 */
	phi = phi + p * P2PHI;
	theta = theta + q * Q2THETA;

	/*
	 * adjust for bias
	 * 
	 * Subtract the weighted difference between the 
	 * measured attitude, and the previous estimate. 
	 */
	phi = phi - (sphi - phi) / C1;
	theta = theta - (stheta - theta) / C1;

	/*
	 * update the rate bias estimation.
	 *
	 * Uses the weighted difference between the estimated 
	 * attitude and the measured attitude.
	 */
	p_b = p_b + (phi - sphi) / C2;
	q_b = q_b + (theta - stheta) / C2;
}

void get_raw_attitude() {
	//only on first call
	if(previous_sample_time == 0) {
		previous_sample_time = get_time_us();
		return;
	} else {
		int32_t p_temp = p;
		period_ms = (get_time_us() - previous_sample_time)/1000;
		sphi += (period_ms * (p/16400))* ATTITUDE_BITS_PER_DEGREE;
		stheta += (period_ms * (q/16400)) * ATTITUDE_BITS_PER_DEGREE;
	}
}



