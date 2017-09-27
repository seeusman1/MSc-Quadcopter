#ifndef __CALIBRATION_CALIBRATION_H_
#define __CALIBRATION_CALIBRATION_H_
#include "../in4073.h"
#include "../statemanager/statemanager.h"
//calibration offsets for the corresponding sensor values.
int16_t sp_offset, sq_offset, sr_offset;
int16_t sax_offset, say_offset, saz_offset;

//is true iff the IMU has been calibrated since boot
bool calibrated = false;


/*
 * Author Rutger van den Berg
 * calibrates a single value.
 */
inline int16_t calibrate_value(int16_t raw, int16_t offset) {
	int32_t calibrated = (int32_t) raw + (int32_t) offset;
	if(calibrated > INT16_MAX) {
		return INT16_MAX;
	}
	if(calibrated < -INT16_MAX) {
		return -INT16_MAX;
	}
	return calibrated;
}


/*
 * Author: Rutger van den Berg
 * 
 * Handles sensor calibration(one point calibration). Has two modes:
 * 	- In calibration mode, the current measured sensor data 
 * 		is used to calculate the calibration offset. 
 *  - In any other mode, the offset is applied to the current measured sensor data.
 */
inline void calibrate() {
	if(get_current_state() == CALIBRATION) {
		sp_offset = - sp;
		sq_offset = - sq;
		sr_offset = - sr;

		sax_offset = -sax;
		say_offset = -say;
		saz_offset = -saz;
	} else {
		sp = calibrate_value(sp, sp_offset);
		sq = calibrate_value(sq, sq_offset);
		sr = calibrate_value(sr, sr_offset);

		sax = calibrate_value(sax, sax_offset);
		say = calibrate_value(say, say_offset);
		saz = calibrate_value(saz, saz_offset);

		calibrated = true;
	}
}


#endif //__CALIBRATION_CALIBRATION_H_