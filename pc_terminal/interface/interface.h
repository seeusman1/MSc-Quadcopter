#ifndef __INTERFACE_H
#define __INTERFACE_H

#define JOYSTICK_PRESENT

#include "../../protocol.h"

/*------------------------------------------------------------
 * Serial
 *------------------------------------------------------------
 */
void rs232_open();

void rs232_close();

int	rs232_getchar_nb();

int rs232_getchar();

int rs232_putchar(char c);

void	term_initio();

void	term_exitio();

void	term_puts(char *s);

void	term_putchar(char c);

int	term_getchar_nb();

int	term_getchar();

/*------------------------------------------------------------
 * Messaging
 *------------------------------------------------------------
 */

void send_message(char *msg);


/*------------------------------------------------------------
 * Joystick/Keyboard Infastructure
 *------------------------------------------------------------
 */
int joy_init();

int joy_read(int * axis, int * button, int fd);

JoystickMessage rs232_createMsg_joystick (int axis[], JoystickPose pose);

JoystickPose calculate_pose(int axis[], int button[]);

void joy_handler(union sigval val);

void *key_thread();

#endif //__INTERFACE_H
