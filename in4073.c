/*------------------------------------------------------------------
 *  in4073.c -- test QR engines and sensors
 *
 *  reads ae[0-3] uart rx queue
 *  (q,w,e,r increment, a,s,d,f decrement)
 *
 *  prints timestamp, ae[0-3], sensors to uart tx queue
 *
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  June 2016
 *------------------------------------------------------------------
 */

#include "in4073.h"
#include "protocol.h"
#include "assert.h"
#include "statemanager/statemanager.h"
#include "communication/communication.h"
#include "calibration/calibration.h"
#include "safety/safety.h"
#include "logging/logging.h"
/*------------------------------------------------------------------
 * main -- everything you need is here :)
 *------------------------------------------------------------------
 */
int main(void)
{
	uart_init();
	gpio_init();
	timers_init();
	adc_init();
	twi_init();
	imu_init(true, 100);	
	baro_init();
	spi_flash_init();
	ble_init();
	init_statemanager();
	uint32_t counter = 0;
	demo_done = false;
	current_pose.lift = 0;
	current_pose.yaw = 0;
	current_pose.roll = 0;
	current_pose.pitch = 0;

	pose_offsets.lift = 0;
	pose_offsets.yaw = 0;
	pose_offsets.roll = 0;
	pose_offsets.pitch = 0;

	uint32_t current_spi_address = LSA;
	uint32_t current = LSA;
	LoggedData data;

	while (!demo_done)
	{
		handle_communication();

		// process_key( dequeue(&rx_queue) );

		if (check_timer_flag()) 
		{
			if (counter++%20 == 0) nrf_gpio_pin_toggle(BLUE);

			adc_request_sample();
			read_baro();

			printf("%10ld | ", get_time_us());
			printf("%3d %3d %3d %3d | ",motor[0],motor[1],motor[2],motor[3]);
			printf("%6d %6d %6d | ", phi, theta, psi);
			printf("%6d %6d %6d | ", sp, sq, sr);
			printf("%4d | %4ld | %6ld", bat_volt, temperature, pressure);
			printf("| %u \n", get_current_state());
			// printf("Motor setpoints are now: %d %d %d %d\n\n", motor[0], motor[1], motor[2], motor[3]);
			clear_timer_flag();

			/*Logging*/
			prepare_to_Log(&data,get_current_state(),ae,phi,theta,psi,sp,sq,sr,motor,pressure,temperature,bat_volt);
				
			if((current_spi_address = log_data(current_spi_address,&data)) == 0){
				printf("Fail to log_data\n");
				nrf_gpio_pin_toggle(RED);
			}else{
				send_logger_flag = 1;
				nrf_gpio_pin_toggle(YELLOW);
			}
			
		}

		if (check_sensor_int_flag()) 
		{
			get_dmp_data();
			calibrate_imu();
			
			run_filters_and_control();
		}
	}	

	//Sends the log to the PC after flight is done
	if (send_logger_flag){
		while(current != current_spi_address)
		{	
			if(check_timer_flag()){
				nrf_gpio_pin_toggle(YELLOW);
				current = send_log_data(current);
				clear_timer_flag();	
			}	
		}
	printf("Uploading DONE!\n");
	}

	printf("\n\t Goodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();
}
