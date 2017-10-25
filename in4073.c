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
#include "telemetry/telemetry.h"
#include "filtering/kalman.h"
#include "filtering/butterworth.h"
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
	imu_init(true, SENSOR_DMP_FEQUENCY);
	baro_init();
	spi_flash_init();
	ble_init();
	init_statemanager();

	demo_done = false;
	uint32_t old_t = 0;


	nrf_wdt_reload_request_enable(NRF_WDT_RR0);
	nrf_wdt_reload_value_set(32768);
	nrf_wdt_task_trigger(NRF_WDT_TASK_START);


	current_pose.lift = -32768;
	current_pose.yaw = 0;
	current_pose.roll = 0;
	current_pose.pitch = 0;

	pose_offsets.lift = 0;
	pose_offsets.yaw = 0;
	pose_offsets.roll = 0;
	pose_offsets.pitch = 0;

	uint32_t write_address = LSA;
	uint32_t current = LSA;
	LoggedData data;

	while (!demo_done)
	{
		handle_communication();

		// process_key( dequeue(&rx_queue) );

		if (check_timer_flag()) 
		{

			adc_request_sample();
			read_baro();

			
			clear_timer_flag();
		}

		if (check_sensor_int_flag()) 
		{
			if(is_raw()) {
				get_raw_sensor_data();
				get_raw_attitude();
				calibrate_imu();
				kalman_filter();
			} else {
				get_dmp_data();

				calibrate_imu();
				phi = sphi;
				theta = stheta;
				psi = spsi;
			}
			
			pressure = bw_filter((int32_t) (pressure));
			check_safety();
			run_filters_and_control();
			send_telemetry();
		}

		

		/*Logging*/
		if ((get_time_us() - old_t) > LOG_FREQ)
		{	
			old_t = get_time_us();

			prepare_to_Log(&data,get_current_state(),ae,phi,theta,psi,sp,sq,sr,motor,pressure,temperature,bat_volt);
			if((write_address = log_data(write_address,&data)) == 0){
				printf("Fail to log_data\n");
				nrf_gpio_pin_toggle(RED);
			} else{
				send_logger_flag = 1;
				nrf_gpio_pin_toggle(YELLOW);
			}

		}
		
			
	}	

	//Sends the log to the PC after flight is done
	printf("Uploading...");
	if (send_logger_flag)
	{
		while(current != write_address)
		{
			if ((get_time_us() - old_t) > SEND_FREQ) 
			{
				old_t = get_time_us();
				nrf_gpio_pin_toggle(YELLOW);
				nrf_gpio_pin_toggle(BLUE);
				nrf_gpio_pin_toggle(RED);
				nrf_gpio_pin_toggle(GREEN);
				current = send_log_data(current);	
			}
		}
			//printf(".");	
	}
	printf("\nDone!\n");

	printf("\nGoodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();
}
