/*------------------------------------------------------------
 * Simple pc terminal in C
 *
 * Arjan J.C. van Gemund (+ mods by Ioannis Protonotarios)
 *
 * read more: http://mirror.datenwolf.net/serial/
 *------------------------------------------------------------
 */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <pthread.h>
#include "pc_terminal.h"

/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */
int main(int argc, char **argv)
{
	
	//char	ck;
	term_puts("\nTerminal program - Embedded Real-Time Systems\n");

	term_initio();
	rs232_open();
	serial_device = 0;
	init_queue(&receive_queue);
	
	/*Joystic initialize
	*/
	pthread_t j_thrd,k_thrd;

	/*open a file for the log
	*/ 
	f = fopen("file.txt", "w");
	
	if (f == NULL){
    	printf("Error opening file!\n");
    	return 1;
	}

	term = 0;
	term_puts("Type ^C to exit\n");

	/* discard any incoming text
	 */
	while ((c = rs232_getchar_nb()) != -1)
		fputc(c,stderr);
	


	if(pthread_create(&k_thrd, NULL, key_thread, NULL)) {

		fprintf(stderr, "Error creating thread\n");
		return 1;

	}

	#ifdef JOYSTICK_PRESENT
	fd = joy_init();

	if(pthread_create(&j_thrd, NULL, joy_thread, NULL)) {

		fprintf(stderr, "Error creating thread\n");
		return 1;

	}	
	#endif
	/* send & receive
	 */
	while(!term)	
	{	
		incoming_msg_check();
	}

	/*wait for the log to download.
	*/
	printf("Downloading..\n");
	while(incoming_msg_check());
	printf("Done!\n");
	
	/*join threads
	*/
	if(pthread_join(k_thrd, NULL)) {

		fprintf(stderr, "Error joining thread\n");
		return 2;

	}
	#ifdef JOYSTICK_PRESENT
	if(pthread_join(j_thrd, NULL)) {

		fprintf(stderr, "Error joining thread\n");
		return 2;

	}
	#endif
	fclose(f);
	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");

	return 0;
}

