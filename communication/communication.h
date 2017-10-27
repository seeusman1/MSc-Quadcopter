#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H
#include "protocol.h"
void handle_communication();
/*
 * Author Rutger van den Berg
 *
 * Sends the provided message to the PC.
 */
void send_message(GenericMessage* msg);

/*
 * Author Rutger van den Berg
 *
 * Sends the provided message to the PC.
 *
 * Does NOT use CRC message integrity.
 */
void send_message_unsafe(GenericMessage* msg);
#endif //__COMMUNICATION_H
