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
	MODE,
	PRINT,
	LOG,
	MOTOR,
	ANGLE,
	RATE,
	STAT
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

/*
* Author D.Patoukas
*/
typedef struct {
	MessageId id : 8;
	char mode;
	char padding[7];
} __attribute__((packed)) ModeMessage;
_Static_assert(sizeof(ModeMessage) == MESSAGE_SIZE, "ModeMessage size is incorrect.");

typedef struct {
	MessageId id : 8;
	char data[PAYLOAD_SIZE];
} __attribute__((packed)) PrintMessage;
_Static_assert(sizeof(PrintMessage) == MESSAGE_SIZE, "PrintMessage size is incorrect.");

typedef struct {
	MessageId id : 8;
	char data[PAYLOAD_SIZE];
} __attribute__((packed)) LogMessage;
_Static_assert(sizeof(LogMessage) == MESSAGE_SIZE, "LogMessage size is incorrect.");

typedef struct {
	MessageId id : 8;
	uint16_t motor[4];
} __attribute__((packed)) MotorMessage;
_Static_assert(sizeof(MotorMessage) == MESSAGE_SIZE, "MotorMessage size is incorrect.");

typedef struct {
	MessageId id : 8;
	int16_t phi;		
	int16_t theta;		
	int16_t psi;
	int8_t padding[2];		
} __attribute__((packed)) AngleMessage;
_Static_assert(sizeof(AngleMessage) == MESSAGE_SIZE, "AngleMessage size is incorrect.");

typedef struct {
	MessageId id : 8;
	int16_t sp;			
	int16_t sq;		
	int16_t sr;		
	int8_t padding[2];		
} __attribute__((packed)) RateMessage;
_Static_assert(sizeof(RateMessage) == MESSAGE_SIZE, "RateMessage size is incorrect.");

typedef struct {
	MessageId id : 8;
	int32_t temperature;
	uint16_t bat_volt; 
	uint8_t mode;			
	uint8_t padding[1];		
} __attribute__((packed)) StatMessage;
_Static_assert(sizeof(StatMessage) == MESSAGE_SIZE, "RateMessage size is incorrect.");

#endif //__PROTOCOL_H
