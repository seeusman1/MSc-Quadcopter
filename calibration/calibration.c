#include "calibration.h"

//calibration offsets for the corresponding sensor values.
int16_t sp_offset, sq_offset, sr_offset;
int16_t sax_offset, say_offset, saz_offset;
int16_t phi_offset, theta_offset, psi_offset;

//is true iff the IMU has been calibrated since boot
bool calibrated = false;

bool is_calibrated() {
	return calibrated;
}



/*
 * Author Rutger van den Berg
 * calibrates a single value.
 */
int16_t calibrate_value(int16_t raw, int16_t offset) {
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
void calibrate_imu() {
	if(get_current_state() == CALIBRATION) {
		sp_offset = - sp;
		sq_offset = - sq;
		sr_offset = - sr;

		sax_offset = -sax;
		say_offset = -say;
		saz_offset = -saz;

		phi_offset = -sphi;
		theta_offset = -stheta;
		psi_offset = -spsi;

		calibrated = true;
	} else {
		sp = calibrate_value(sp, sp_offset);
		sq = calibrate_value(sq, sq_offset);
		sr = calibrate_value(sr, sr_offset);

		sax = calibrate_value(sax, sax_offset);
		say = calibrate_value(say, say_offset);
		saz = calibrate_value(saz, saz_offset);

		phi = calibrate_value(sphi, phi_offset);
		theta = calibrate_value(stheta, theta_offset);
		psi = calibrate_value(spsi, psi_offset);
	}
}

/*
 * Author: Rutger van den Berg
 * 
 * Handles joystick calibration(one point calibration).
 */
void calibrate_js() {
	current_pose.lift = calibrate_value(current_pose.lift, pose_offsets.lift);
	current_pose.roll = calibrate_value(current_pose.roll, pose_offsets.roll);
	current_pose.pitch = calibrate_value(current_pose.pitch, pose_offsets.pitch);
	current_pose.yaw = calibrate_value(current_pose.yaw, pose_offsets.yaw);
}

/*
 * Author: Rutger van den Berg
 * Reset the calibration state. Forgets about all saved values. 
 * is_calibrated() will return false until the calibration routing has been used again.
 */
void reset_calibration() {
	sp_offset = 0;
	sq_offset = 0;
	sr_offset = 0;
	sax_offset = 0;
	say_offset = 0;
	saz_offset = 0;
	phi_offset = 0;
	theta_offset = 0;
	psi_offset = 0;
	calibrated = false;
}
