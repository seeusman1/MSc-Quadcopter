#ifndef __CALIBRATION_CALIBRATION_H_
#define __CALIBRATION_CALIBRATION_H_
#include "../in4073.h"
#include "../statemanager/statemanager.h"


/*
 * Author: Rutger van den Berg
 * 
 * Handles sensor calibration(one point calibration). Has two modes:
 * 	- In calibration mode, the current measured sensor data 
 * 		is used to calculate the calibration offset. 
 *  - In any other mode, the offset is applied to the current measured sensor data.
 */
void calibrate_imu();

/*
 * Author: Rutger van den Berg
 * 
 * Handles joystick calibration(one point calibration).
 */
void calibrate_js();

/*
 * Author: Rutger van den Berg
 * Returns true iff the sensors have been calibrated.
 */
bool is_calibrated();
#endif //__CALIBRATION_CALIBRATION_H_