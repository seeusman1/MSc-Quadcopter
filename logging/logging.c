#include <string.h> 
#include <assert.h>
#include "../protocol.h"
#include "../in4073.h"
#include "logging.h"
/*
*Author D.Patoukas
*/
/********************LOGGER-INFASTRUCTURE***************/

/*
*D.Patoukas 
*Create a message structure for the data to be stored.
*data argument is the pointer to the data structure LoggedData
*
*/
void prepare_to_Log(	LoggedData *data,
						uint8_t mode,
						int16_t *ae, 
						uint16_t phi, uint16_t theta, uint16_t psi, 
						uint16_t sp, uint16_t sq, uint16_t sr)
{
	//Store the data
	data->timestamp = get_time_us();
	data->mode = mode;
	memcpy(data->ae,ae,AELEN*sizeof(uint16_t));
	data->phi = phi;
	data->theta = theta;
	data->psi = psi;
	data->sp = sp;
	data->sq = sq;
	data->sr = sr;
}
/*
*D.Patoukas 
*Logs the data into the SPI flash memory.
*@argument address specifies starting address for the data to be stored
*@argument data pointer to the data structure LoggedData
*
*/


uint32_t log_data(uint32_t address, LoggedData *data)
{	
	if(flash_write_bytes( address, (uint8_t *) &(data->timestamp), sizeof(LoggedData))){
		address += sizeof(LoggedData);
		return address;
	}
	else{	
		return 0;
	}
}

/*
*D.Patoukas 
*Logs the data into the SPI flash memory.
*@argument address specifies starting address for the data to be send
*
*/
uint32_t send_log_data(uint32_t address)//, LoggedData *data)
{
	int i;
	LogMessage msg;
	LoggedData data;
	msg.id = LOG;
	//Read data from SPI-FLASH
	if (flash_read_bytes(address,(uint8_t*) &data, sizeof(LoggedData)))
	{	
		msg.data = data;
		char* ptr =(char*) &msg;
		//Send the data
		for (i = 0; i < sizeof(LogMessage); ++i)
		{	
			nrf_gpio_pin_toggle(GREEN);
			uart_put(*ptr++);
		}
		address += sizeof(LoggedData);
		return address;
	}else{
		nrf_gpio_pin_toggle(RED);
		return address;
	}
}