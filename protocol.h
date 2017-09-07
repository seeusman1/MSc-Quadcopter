#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include <stdint.h>


typedef enum {
	JOYSTICK,
	MODE
} MessageId;

#define PAYLOAD_SIZE 8
#define MESSAGE_SIZE 9

typedef struct {
	int16_t lift;
	int16_t roll;
	int16_t pitch;
	int16_t yaw;
} JoystickPose;

typedef struct {
	MessageId id;
	JoystickPose phi;
} __attribute__((packed)) JoystickMessage;
_Static_assert(sizeof(JoystickMessage) == MESSAGE_SIZE, "JoystickMessage size is incorrect.");


typedef struct {
	MessageId id;
	char mode;
	char padding[7];
} ModeMessage;
_Static_assert(sizeof(ModeMessage) == MESSAGE_SIZE, "ModeMessage size is incorrect.");
#endif //__PROTOCOL_H