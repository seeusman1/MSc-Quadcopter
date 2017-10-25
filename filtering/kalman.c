#include "kalman.h"
#include "../in4073.h"
#include "fixedptc.h"

fixedptd P2PHI = 0;

fixedptd C1;
fixedptd C2;

extern int16_t p, q;			// Angular rate output. filtered.
extern int16_t phi, theta; 		// Attitude output. filtered.
extern int16_t sp, sq; 			// Angular rate input. updated by Gyro
extern int16_t sphi, stheta;	// Attitude input. updated by ?
;
int16_t p_b, q_b;				//Estimated angular rate bias

void kalman_init() {
	C1 = fixedpt_fromint(100);
	C2 = fixedpt_fromint(1000000);
	fixedptd MAX_VALUE_ATTITUDE = fixedpt_fromint(32768);
	fixedptd MAX_DEGREES_ATTITUDE = fixedpt_fromint(90);
	fixedptd ATTITUDE_BITS_PER_DEGREE = fixedpt_div(MAX_VALUE_ATTITUDE, MAX_DEGREES_ATTITUDE);
	fixedptd P_RESOLUTION = fixedpt_div(fixedpt_fromint(164), fixedpt_fromint(10));
	fixedptd P2DEGREE_DIVIDER = fixedpt_div(P_RESOLUTION, fixedpt_fromint(SENSOR_RAW_FREQUENCY));
	P2PHI = fixedpt_div(ATTITUDE_BITS_PER_DEGREE,P2DEGREE_DIVIDER);
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
	phi = phi + fixedpt_toint(fixedpt_mul(fixedpt_fromint(p), P2PHI));
	theta = theta + fixedpt_toint(fixedpt_mul(fixedpt_fromint(q), P2PHI));

	/*
	 * adjust for bias
	 * 
	 * Subtract the weighted difference between the 
	 * measured attitude, and the previous estimate. 
	 */
	phi = phi - fixedpt_toint(fixedpt_div(fixedpt_fromint((sphi - phi)), C1));
	theta = theta - fixedpt_toint(fixedpt_div(fixedpt_fromint((stheta - theta)), C1));

	/*
	 * update the rate bias estimation.
	 *
	 * Uses the weighted difference between the estimated 
	 * attitude and the measured attitude.
	 */
	p_b = p_b + fixedpt_toint(fixedpt_div(fixedpt_fromint((phi - sphi)), C2));
	q_b = q_b + fixedpt_toint(fixedpt_div(fixedpt_fromint((theta - stheta)), C2));
}

void get_raw_attitude() {
	sphi += fixedpt_toint(fixedpt_mul(fixedpt_fromint(p), P2PHI));
	stheta += fixedpt_toint(fixedpt_mul(fixedpt_fromint(q), P2PHI));
}



