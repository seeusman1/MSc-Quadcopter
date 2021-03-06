#include "safety.h"
#include "in4073.h"
#include "../statemanager/statemanager.h"
uint8_t bat_exceeded = 0;
const uint8_t BAT_EXCEEDED_THRESHOLD = 20;
/* 
 * Author Rutger van den Berg
 * Checks safety conditions and switches to emergency mode if one of them is violated.
 */ 
void check_safety() {
	if(get_current_state() == PANIC) {
		//Don't re-check safety when we're already in panic mode.
		return;
	}
	bool safe = true;
	safe &= check_battery();
	if (!safe) {
		try_transition(PANIC);
		printf("[SAFETY] One or more safety conditions were violated, switching to panic.\n");
	}
}

/*
 * Checks whether the battery voltage is sufficient for safe operation.
 * Returns true iff the voltage is above the threshold specified in the header.
 */
bool check_battery() {
	if(get_current_state() > SAFE && bat_volt < BAT_THRESHOLD) {
		++bat_exceeded;
		if(bat_exceeded > BAT_EXCEEDED_THRESHOLD) {
			printf("[SAFETY] Battery voltage too low.\n");
			return false;
		}
	} else {
		bat_exceeded = 0;
	}
	return true;
}
