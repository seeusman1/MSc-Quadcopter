#include <assert.h>

#include "communication.h"
#include "../in4073.h"
#include "../crc/crc.h"
#include "../statemanager/statemanager.h"
#include "../protocol.h"
#include "calibration/calibration.h"
/*------------------------------------------------------------------
 * process_key -- process command keys
 *------------------------------------------------------------------
 */
void process_key(uint8_t c)
{
	switch (c)
	{
		case 'q':
			pose_offsets.yaw += JS_CALIBRATION_STEP;
			break;
		case 'a':
			pose_offsets.lift += JS_CALIBRATION_STEP;
			break;
		case 'z':
			pose_offsets.lift -= JS_CALIBRATION_STEP;
			break;
		case 'w':
			pose_offsets.yaw -= JS_CALIBRATION_STEP;
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
		case 'u':
			P += 1;
			break;
		case 'j':
			if(P > 0) {
				P -= 1;
			}
			break;

		case 'i':
			P1 += 1;
			break;
		case 'k':
			if(P1 > 0) {
				P1 -= 1;
			}
			break;
		case 'o':
			P2 += 1;
			break;
		case 'l':
			if(P2 > 0) {
				P2 -= 1;
			}
			break;
		case 't':
			P_height += 2;
			break;
		case 'g':
			if(P_height > 1) {
				P_height -= 2;
			}
			break;


		case 28:
			//UP
			pose_offsets.pitch += JS_CALIBRATION_STEP;
			break;
		case 29:
			//DOWN
			pose_offsets.pitch -= JS_CALIBRATION_STEP;
			break;
		case 30:
			//RIGHT
			pose_offsets.roll += JS_CALIBRATION_STEP;
			break;
		case 31:
			//LEFT
			pose_offsets.roll -= JS_CALIBRATION_STEP;
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
		case '6':
			if(!get_current_state() == SAFE) {
				printf("May only switch to raw mode in SAFE mode.\n");
				break;
			}
			set_raw(!is_raw());
		case '7':
			try_transition(HEIGHTCONTROL);
			break;
		case 27:
			try_transition(SAFE);
			demo_done = true;
			break;
		case (char) 4: //ASCII end of transmission, using it for killswitch.
			try_transition(PANIC);
			break;
		default:
			nrf_gpio_pin_toggle(RED);
	}
}



/*
 * Author: Rutger van den Berg
 *  
 * Handles a single message. 
 * Identifies the type of message, and calls the appropriate message specific handler.  
 */

void handle_message(GenericMessage *message) 

{
	switch (message->id)
	{
		case JOYSTICK:
		{
			JoystickMessage *joymsg = (JoystickMessage*) message;
			current_pose = joymsg->pose;
			current_pose.yaw = -current_pose.yaw; 
			calibrate_js();

			//this is also used as a heartbeat, so reset the watchdog timer.
			nrf_wdt_reload_request_set(NRF_WDT_RR0);
			break;
		}
		case MODE:
		{
			ModeMessage *modemsg = (ModeMessage*) message;
			process_key(modemsg->mode);
			break;
		}
		default:
			printf("Received unknown message type: %d.\n", message->id);
	}
}

// Finding header
/*
 * Author Muhammad Usman Saleem
 * 
 * Finds a CRC header by dequeueing bytes until one matches the CRC header. 
 */
bool detect_header()
{
	uint8_t head;
	
	while (rx_queue.count >= MESSAGE_SIZE)
	{
		head = dequeue(&rx_queue);
		if (head == CRC_HEADER) {

			return true;
		}
		
	}
	return false;
}

/*
 * Author Rutger van den Berg
 * Read a single message from the UART rx buffer.
 */ 
 bool read_message(CRCMessage *message) {
	//check to make sure there are enough bytes in the buffer to create a message.
	assert(rx_queue.count >= CRC_MESSAGE_SIZE-1);
	while (true) {
		//find a CRC header, if none can be found return false
		if(!detect_header()) {
			printf("No header detected!\n");
			return false;
		}	
		message->header = CRC_HEADER;

		//read the message 
		for (uint16_t i = 0; i < CRC_PAYLOAD_SIZE; i++) {
			message->payload[i] = peek(&rx_queue, i);
		}
		//read the crc
		message->crc = peek(&rx_queue, CRC_PAYLOAD_SIZE);

		/* 
		 * verify the CRC. If the crc is correct, 
		 * a message has been successfully read and we can dequeue the bytes.
		 * If not, there was a transmission error so try again. 
		 */
		if (verify_crc(message)) {
			for(uint8_t i = 0; i <= CRC_PAYLOAD_SIZE; i++) {
				dequeue(&rx_queue);
			}
			return true;
		}
		printf("[CRC] Transmission error detected.\n");
	}
}
/*
 * Author Muhammad Usman Saleem
 */
void handle_communication() {
	CRCMessage message;
	
	//while complete messages are in the buffer, read them.
	while (rx_queue.count >= CRC_MESSAGE_SIZE) {
		if(read_message(&message)) {
			GenericMessage *msg = (GenericMessage*) &(message.payload[0]);
			handle_message(msg);
		}
	}
}

/*
 * Author Rutger van den Berg
 *
 * Sends the provided message to the PC.
 */
void send_message(GenericMessage* msg) {
	CRCMessage message = make_packet((char*) msg);
	for (uint8_t i = 0; i < CRC_MESSAGE_SIZE; i++)
	{
		uart_put(((char*) (&message))[i]);
	}
}

/*
 * Author Rutger van den Berg
 *
 * Sends the provided message to the PC.
 *
 * Does NOT use CRC message integrity.
 */
void send_message_unsafe(GenericMessage* msg) {
	for (uint8_t i = 0; i < MESSAGE_SIZE; i++)
	{
		uart_put(((char*) (msg))[i]);
	}
}
