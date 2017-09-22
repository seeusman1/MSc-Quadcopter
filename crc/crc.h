#ifndef __CRC_H
#define __CRC_H
#include <string.h>
#include <stdbool.h>

#define CRC_HEADER ((uint8_t)0x0F)
#define CRC_MESSAGE_SIZE 11
#define CRC_POLYNOMIAL ((uint8_t)0x2F)
#define CRC_PAYLOAD_SIZE 9

typedef struct {
	uint8_t header;
	uint8_t payload[CRC_PAYLOAD_SIZE];
	uint8_t crc;
} __attribute__ ((__packed__)) CRCMessage;
_Static_assert(sizeof(CRCMessage) == CRC_MESSAGE_SIZE, "CRCMessage size incorrect.");


//Computing CRC bits
/*
 * Computes 8bit CRC for CRC_PAYLOAD_SIZE bytes starting at the provided pointer.
 */
uint8_t compute_crc8 (char *payload)
{
	char crc = 0;
	int byte;
	for (byte =0; byte <(CRC_MESSAGE_SIZE-1); ++byte)
	{
		crc ^= (char) (payload[byte]);
		
		for (uint8_t bit =0; bit<8;bit++)
		{
			if ((crc & 0x80) !=0)
			{
				crc = (uint8_t) ((crc << 1) ^ CRC_POLYNOMIAL);
			}
			else
				crc <<=1;
		}
	}
	return crc;
}
/*
 * Returns true iff the crc in the message is correct. 
 * 
 * Calculates 8bit crc over the header plus payload, and compares it to the crc stored in the message. 
 */
bool verify_crc(CRCMessage *message) {
	//compute crc over the header+payload
	uint8_t crc = compute_crc8((char*) &(message->header));
	if (crc != message->crc) {
		return false;
	} 
	return true;
}
/*
 * Creates a packet with the provided payload.
 * Will assign the header and calculate the crc over the header plus the payload.
 */ 
CRCMessage make_packet(char payload[CRC_PAYLOAD_SIZE]) {
	CRCMessage message;
	message.header = CRC_HEADER;
	memcpy(&(message.payload), &(payload[0]), CRC_PAYLOAD_SIZE);	
	message.crc = compute_crc8((char*) &(message.header));
	
	return message;
}

#endif