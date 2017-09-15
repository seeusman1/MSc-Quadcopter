#ifndef __STATEMANAGER_H
#define __STATEMANAGER_H
#include <stdbool.h>
/*
 * Author Rutger van den Berg
 * Lists all modes the quadcopter can be in.
 */
typedef enum {
	SAFE, 
	PANIC,
	MANUAL,
	CALIBRATION,
	YAWCONTROL,
	FULLCONTROL,
	MODE_COUNT
} __attribute__ ((__packed__)) state_t;
/*
 * Author Rutger van den Berg
 * An array that controls which transitions are allowed. 
 * allowed_transitions[FROM][TO] is true iff the transition is valid.
 */
 extern bool allowed_transitions[MODE_COUNT][MODE_COUNT];

/*
 * Author Rutger van den Berg
 * Check if all pre-conditions for a transition are met.
 */
bool check_conditions(state_t to);

/*
 * Author Rutger van den Berg
 * Attempts to transition to the specified state. 
 * Returns true iff the transition was successful. 
 */
bool try_transition(state_t to);

/*
 * Author: Rutger van den Berg
 * Returns the current state.
 */
state_t get_current_state();

/*
 * Author: Rutger van den Berg
 * Initializes the statemanager. 
 */
void init_statemanager();


#endif //__STATEMANAGER_H