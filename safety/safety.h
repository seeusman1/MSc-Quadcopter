#ifndef __STATEMANAGER_H
#define __STATEMANAGER_H
#include <stdint.h>
/*
 * Author Rutger van den Berg
 * Defines functions to check safety conditions.
 */

//TODO: Check the actual value.
const static uint16_t BAT_THRESHOLD = 1050;

/*
 * Author Rutger van den Berg
 * Checks safety conditions and switches to emergency mode if one of them is violated.
 */
void check_safety();


/*
 * Checks the battery voltage.
 * Returns true iff the voltage is sufficient.
 */
bool check_battery();


#endif //__STATEMANAGER_H