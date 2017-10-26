#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H
void handle_communication();
/*
 * Author Rutger van den Berg
 * Writes 9 bytes from the provided pointer to UART.
 * Assumes that 9 bytes containing a single message can be found at the specified location.
 */
void send_message(char *msg);

/*
 * Author Rutger van den Berg
 * Writes 9 bytes from the provided pointer to UART.
 * Assumes that 9 bytes containing a single message can be found at the specified location.
 *
 * Does NOT apply a CRC message integrity check
 */
void send_message_unsafe(char *msg);
#endif //__COMMUNICATION_H
