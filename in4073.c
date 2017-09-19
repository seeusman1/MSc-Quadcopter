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
/*------------------------------------------------------------------
 * process_key -- process command keys
 *------------------------------------------------------------------
 */
void process_key(uint8_t c)
{
	switch (c)
	{
		case 'q':
			ae[0] += 10;
			break;
		case 'a':
			ae[0] -= 10;
			if (ae[0] < 0) ae[0] = 0;
			break;
		case 'w':
			ae[1] += 10;
			break;
		case 's':
			ae[1] -= 10;
			if (ae[1] < 0) ae[1] = 0;
			break;
		case 'e':
			ae[2] += 10;
			break;
		case 'd':
			ae[2] -= 10;
			if (ae[2] < 0) ae[2] = 0;
			break;
		case 'r':
			ae[3] += 10;
			break;
		case 'f':
			ae[3] -= 10;
			if (ae[3] < 0) ae[3] = 0;
			break;
		case '0':
			try_transition(SAFE);
			break;
		case '1':
			try_transition(PANIC);
			break;
		case '2':
			try_transition(MANUAL);
			break;
		case '3':
			try_transition(CALIBRATION);
			break;
		case '4':
			try_transition(YAWCONTROL);
			break;
		case '5':
			try_transition(FULLCONTROL);
			break;
		case 27:
			demo_done = true;
			break;
		default:
			nrf_gpio_pin_toggle(RED);
	}
}


// Finding header

uint8_t detect_header()
{
	uint8_t head;
	
	while (rx_queue.count >= MESSAGE_SIZE)
	{
		head = dequeue(&rx_queue);
		if (head == MESSAGE_HEADER)
			return 1;
	}
	return 0;
}

// If CRC fails, find next header starting from the same frame
uint8_t detect_next_header(uint8_t *message)
{
	uint8_t i;
	
	for (i=1; i< MESSAGE_SIZE; i++)
	{
		if (message[i] == MESSAGE_HEADER)
		return i;
	}
	return 0;
}


//Checking transmisison errors
uint8_t verify_crc8 (uint8_t *message)
{
	uint8_t crc = 0;
	int byte;
	for (byte =0; byte <(MESSAGE_SIZE-1); ++byte)
	{
		// for inducing transmission error
		//if ((char) message[byte]== 'w') 
		//	message[byte]= 0xFF;
		crc ^= (uint8_t) (message[byte]);
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
	if (message[byte]==crc)
		return 1;
	else
		{
		printf ("%s", "Error in Transmnission detected: ");
		return 0;
		}
}



/*

 * Author: Rutger van den Berg

 * Reads 9 bytes from the UART RX buffer, and handles the message. 

 * Assumes that at least 9 bytes are in the buffer.

 */
/* 
void handle_message() 

{

	assert(rx_queue.count >= MESSAGE_SIZE);

	uint8_t msg[MESSAGE_SIZE];

	//Copy the message out of the buffer.

	for(uint8_t i = 0; i < MESSAGE_SIZE; i++) {

		msg[i] = dequeue(&rx_queue);

	}

	switch (msg[0]) //first byte is the message ID

	{

		case JOYSTICK:

		{

			JoystickMessage *joymsg = (JoystickMessage*) &msg[0];

			printf("Joystick pose is now: %d %d %d %d\n\n", joymsg->pose.lift, 

				joymsg->pose.roll, joymsg->pose.pitch, joymsg->pose.yaw);

			break;

		}

		case MODE:

		{

			ModeMessage *modemsg = (ModeMessage*) msg;

			process_key(modemsg->mode);

			break;

		}

		default:

			printf("Received unknown message type: %d.\n", msg[0]);

	}

} */


void handle_message() 
{
	assert(rx_queue.count >= MESSAGE_SIZE);
	uint8_t msg[MESSAGE_SIZE];
	
	if (!detect_header())
		return; // discard frame
	
	msg[0] = MESSAGE_HEADER;
	
	for(uint8_t i = 1; i < MESSAGE_SIZE; i++) {
		msg[i] = dequeue(&rx_queue);
	}
	
	while (!verify_crc8 (msg)) // checking transmission errors
	{
		uint8_t i = detect_next_header (msg);// drop byte and find next header
		
		if (!i) 
		{
			printf("%d bytes droped\n", MESSAGE_SIZE);
			return; // discard frame if no other MESSAGE_HEADER is present
		}
		
		if (!(rx_queue.count >= i))
		{
			printf("%d bytes droped\n", MESSAGE_SIZE);
			return; // if adding remaining queue bytes can't make a complete MESSAGE_SIZE
		}
		
		printf("%d bytes droped\n", i);	
		msg[0] = msg[i]; // MESSAGE_HEADER
		
		uint8_t j;
		
		for (j=1 ; j<(MESSAGE_SIZE-i);j++)
			msg[j] = msg[i+j]; //shifting bytes from previous frame
		
		for (j=j;j<MESSAGE_SIZE;j++)
			msg[j] = dequeue(&rx_queue); // reading new bytes from queue
	
	}
	
	switch (msg[1]) //first byte is the message ID
	{
		case JOYSTICK:
		{
			JoystickMessage *joymsg = (JoystickMessage*) &msg[0];
			printf("Joystick pose is now: %d %d %d %d\n\n", joymsg->pose.lift, 
				joymsg->pose.roll, joymsg->pose.pitch, joymsg->pose.yaw);
			break;
		}
		case MODE:
		{
			ModeMessage *modemsg = (ModeMessage*) msg;
			process_key(modemsg->mode);
			break;
		}
		default:
			printf("Received unknown message type: %d.\n", msg[0]);
	}
}


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

	while (!demo_done)
	{
		//If a full message is in the buffer, read it. 
		
		if (rx_queue.count >= MESSAGE_SIZE) {
			handle_message();
			
		}
		for (int i = 0; i < 1000000; i++) {

		}
		// process_key( dequeue(&rx_queue) );

		if (check_timer_flag()) 
		{
			if (counter++%20 == 0) nrf_gpio_pin_toggle(BLUE);

			adc_request_sample();
			read_baro();

			printf("%10ld | ", get_time_us());
			printf("%3d %3d %3d %3d | ",ae[0],ae[1],ae[2],ae[3]);
			printf("%6d %6d %6d | ", phi, theta, psi);
			printf("%6d %6d %6d | ", sp, sq, sr);
			printf("%4d | %4ld | %6ld", bat_volt, temperature, pressure);
			printf("| %u \n", get_current_state());

			clear_timer_flag();
		}

		if (check_sensor_int_flag()) 
		{
			get_dmp_data();
			run_filters_and_control();
		}
	}	

	printf("\n\t Goodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();
}
