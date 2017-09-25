#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include <stdint.h>


/*
 * Author: Rutger van den Berg
 * Since this code will be shared between host and target, 
 * ensure that everything is packed and assert all assumptions on sizes.
 */
typedef enum {
	JOYSTICK,
	MODE
} __attribute__ ((__packed__)) MessageId;
_Static_assert(sizeof(MessageId) == 1, "MessageId size is incorrect.");

#define PAYLOAD_SIZE 8
#define MESSAGE_SIZE 9

typedef struct {
	MessageId id : 8;
	char padding[PAYLOAD_SIZE];
} __attribute__ ((__packed__)) GenericMessage;
_Static_assert(sizeof(GenericMessage) == MESSAGE_SIZE, "GenericMessage size is incorrect.");


typedef struct {
	int16_t lift;
	int16_t roll;
	int16_t pitch;
	int16_t yaw;
} __attribute__((packed)) JoystickPose;
_Static_assert(sizeof(JoystickPose) == PAYLOAD_SIZE, "JoystickPose size is incorrect.");


//TODO: Maybe modify this to have a single Message with an id and a union of types of payload.
typedef struct {
	MessageId id : 8;
	JoystickPose pose;
} __attribute__((packed)) JoystickMessage;
_Static_assert(sizeof(JoystickMessage) == MESSAGE_SIZE, "JoystickMessage size is incorrect.");


typedef struct {
	MessageId id : 8;
	char mode;
	char padding[7];
} ModeMessage;
_Static_assert(sizeof(ModeMessage) == MESSAGE_SIZE, "ModeMessage size is incorrect.");
#endif //__PROTOCOL_H