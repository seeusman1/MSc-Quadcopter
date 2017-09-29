#include "safety.h"
#include "in4073.h"
#include "../statemanager/statemanager.h"

/* 
 * Author Rutger van den Berg
 * Checks safety conditions and switches to emergency mode if one of them is violated.
 */ 
void check_safety() {
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
		return false;
	}
	return true;
}