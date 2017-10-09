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
#include <time.h>
#include <signal.h>
#include <errno.h>


#include "pc_terminal.h"







timer_t timerid;
/*
 * Source: http://devlib.symbian.slions.net/s3/GUID-B4039418-6499-555D-AC24-9B49161299F2.html
 */
void setup_js() {
	int Ret;

    pthread_attr_t attr;
    pthread_attr_init( &attr );

    struct sched_param parm;
    parm.sched_priority = 255;
    pthread_attr_setschedparam(&attr, &parm);

    struct sigevent sig;
    sig.sigev_notify = SIGEV_THREAD;
    sig.sigev_notify_function = joy_handler;
    sig.sigev_value.sival_int =20;
    sig.sigev_notify_attributes = &attr;

    //create a new timer.
    
    Ret = timer_create(CLOCK_REALTIME, &sig, &timerid);
    if (Ret == 0) {
        struct itimerspec in, out;
        in.it_value.tv_sec = 1;
        in.it_value.tv_nsec = 0;
        in.it_interval.tv_sec = 0;
        in.it_interval.tv_nsec = 10000000;
        //issue the periodic timer request here.
        Ret = timer_settime(timerid, 0, &in, &out);
        if(Ret != 0) {
            printf("timer_settime() failed with %d\n", errno);
        }

    }
    
}

void close_js() {
	printf("Closing timer.\n");
    //delete the timer.
    timer_delete(timerid);
}


/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */
int main(int argc, char **argv)
{
	

	term_puts("\nTerminal program - Embedded Real-Time Systems\n");

	term_initio();
	rs232_open();
	serial_device = 0;
	init_queue(&receive_queue);
	/*Joystic initialize
	*/

	/*open a file for the log
	*/ 
	f = fopen("file.txt", "w");
	
	if (f == NULL){
    	printf("Error opening file!\n");
    	return 1;
	}

	 pthread_t k_thrd;

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
	setup_js();
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
	fprintf(f,"Mode |sq\t \tsq\t \tsr\t|");
	// fprintf(f, "\tphi theta\t psi|");
	fprintf(f, "%-6s%-8s%-6s", "phi", "theta", "psi");
	fprintf(f, "ae[0]\tae[1]\tae[2]\tae[3]|motor[0]\tmotor[1]\tmotor[2]\tmotor[3]|");
	fprintf(f,"Bat\t ,Temp:\t ,Press:\t|\n");
	while(incoming_msg_check());
	printf("Done!\n");
	
	/*join threads
	*/
	if(pthread_join(k_thrd, NULL)) {

		fprintf(stderr, "Error joining thread\n");
		return 2;

	}

	#ifdef JOYSTICK_PRESENT
	close_js();
	#endif

	fclose(f);
	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");

	return 0;
}

