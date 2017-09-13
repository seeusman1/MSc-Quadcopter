#include "statemanager.h"
bool allowed_transitions [MODE_COUNT][MODE_COUNT];
state_t current_state = SAFE;
// _Static_assert(sizeof(allowed_transitions) == (MODE_COUNT*MODE_COUNT/8)+1, "allowed_transitions unexpectedly large.");

/*
 * Author: Rutger van den Berg
 * Checks if transitioning to state to is legal.
 */ 
bool check_conditions(state_t to) {
	if(!allowed_transitions[current_state][to]) {
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
		return true;
	}
	return false;
}

/*
 * Author: Rutger van den Berg
 */
void init_statemanager() {
	allowed_transitions[SAFE][MANUAL] = true;
	allowed_transitions[MANUAL][SAFE] = true;
}


/*
 * Author: Rutger van den Berg
 */
state_t get_current_state() {
	return current_state;
}