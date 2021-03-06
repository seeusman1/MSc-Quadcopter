#include "statemanager.h"
#include "../in4073.h"
#include "../calibration/calibration.h"
bool allowed_transitions [MODE_COUNT][MODE_COUNT];
state_t current_state = SAFE;
bool _is_raw = false;
/*
 * Author: Rutger van den Berg
 * Checks if transitioning to state to is legal.
 */ 
bool check_conditions(state_t to) {
	if(!allowed_transitions[current_state][to]) {
		return false;
	}

	if(to == YAWCONTROL && !is_calibrated()) {
		return false;
	}
	if(to == FULLCONTROL && !is_calibrated()) {
		return false;
	}
	if(to == HEIGHTCONTROL && !is_calibrated()) {
		return false;
	}
	if ((to == MANUAL || to == YAWCONTROL || to == FULLCONTROL ) &&  current_state != HEIGHTCONTROL && current_pose.lift > -32000) {
		printf("Lift>0\n");
		return false;
	}
	//Add states with specific pre-conditions here
	return true;
}

/*
 * Author: Rutger van den Berg
 * Attempts a transition to the specified state. 
 * Returns true iff the transition was successful. 
 */
bool try_transition(state_t to) {
	//TODO: Semaphore on current_state
	if(check_conditions(to)) {
		current_state = to;
		if (current_state == HEIGHTCONTROL){
			pressure_ref = pressure;
			thrust_ref = current_pose.lift;
		}
			
		return true;
	}
	return false;
}

/*
 * Author: Rutger van den Berg
 * Whitelist all allowed state transitions. 
 */
void init_statemanager() {
	allowed_transitions[SAFE][MANUAL] = true;
	allowed_transitions[MANUAL][SAFE] = true;
	allowed_transitions[MANUAL][PANIC] = true;
	allowed_transitions[SAFE][PANIC] = true;
	allowed_transitions[PANIC][SAFE] = true;
	allowed_transitions[SAFE][CALIBRATION] = true;
	allowed_transitions[CALIBRATION][SAFE] = true;
	allowed_transitions[SAFE][YAWCONTROL] = true;
	allowed_transitions[YAWCONTROL][SAFE] = true;
	allowed_transitions[YAWCONTROL][PANIC] = true;
	allowed_transitions[SAFE][FULLCONTROL] = true;
	allowed_transitions[FULLCONTROL][SAFE] = true;
	allowed_transitions[FULLCONTROL][PANIC] = true;
	allowed_transitions[FULLCONTROL][HEIGHTCONTROL] = true;
	allowed_transitions[HEIGHTCONTROL][SAFE] = true;
	allowed_transitions[HEIGHTCONTROL][PANIC] = true;
	allowed_transitions[HEIGHTCONTROL][FULLCONTROL] = true;

}


/*
 * Author: Rutger van den Berg
 */
state_t get_current_state() {
	return current_state;
}

/*
 * Author: Rutger van den Berg
 * Returns true iff the drone is set to use the IMU in raw mode.
 */
bool is_raw() {
	return _is_raw;
}

/*
 * Author: Rutger van den Berg
 * Attempts to switch the drone to raw mode if raw is true, else attempts to switch to DMP mode.
 */
void set_raw(bool raw) {
	//Only switch sensor modes when in safe mode.
	if (raw != _is_raw && get_current_state() == SAFE) {
		_is_raw = raw;
		if(_is_raw) {
			imu_init(false, SENSOR_RAW_FREQUENCY);
		} else {
			imu_init(true, SENSOR_DMP_FEQUENCY);
		}
		printf("Raw mode is: %u\n", _is_raw);
		reset_calibration();
	}
}
