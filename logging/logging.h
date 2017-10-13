#ifndef __LOGGING_H
#define __LOGGING_H

#define AELEN 4
#define LOGGER_SIZE 40

typedef struct 
{	
	uint8_t padding;// 1 byte
	uint8_t mode;		// 1 byte	
	uint16_t sp;		// 2 bytes	
	uint16_t sq;		// 2 bytes
	uint16_t sr;		// 2 bytes
	
	uint16_t ae[AELEN];	// AELEN*2 = 4*2 = 8 bytes
	
	int16_t motor[AELEN]; //AELEN*2 = 4*2 = 8 bytes

	int32_t pressure;   //4
	int32_t temperature;//4

	uint16_t phi;		// 2 bytes
	uint16_t theta;		// 2 bytes
	uint16_t psi;		// 2 bytes
	uint16_t bat_volt;  // 2 bytes
	
						//LOGGER_SIZE:40 Bytes 
}__attribute__((packed))LoggedData;
_Static_assert(sizeof(LoggedData) == LOGGER_SIZE, "LoggedData size is incorrect.");

void prepare_to_Log(	LoggedData *data,
						uint8_t mode,
						int16_t *ae, 
						uint16_t phi, uint16_t theta, uint16_t psi, 
						uint16_t sp, uint16_t sq, uint16_t sr,
						int16_t *motor,
						int32_t pressure,int32_t temperature,
						uint16_t bat_volt);

uint32_t log_data(uint32_t address, LoggedData *data);

uint32_t send_log_data(uint32_t address);
#endif //__LOGGING_H