#include <assert.h>

#include "communication.h"
#include "../in4073.h"
#include "../crc/crc.h"
#include "../statemanager/statemanager.h"
#include "../protocol.h"
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
		case 'A':
		//UP
		k_off[0] += ARROW_OFFSET;
		break;
		case 'B':
		//DOWN
		k_off[1] -= ARROW_OFFSET;
		break;
		case 'C':
		//RIGHT
		k_off[2] += ARROW_OFFSET;
		break;
		case 'D':
		//LEFT
		k_off[3] -= ARROW_OFFSET;
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