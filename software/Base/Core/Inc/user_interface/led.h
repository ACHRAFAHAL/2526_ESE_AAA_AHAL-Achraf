/*
 * led.h
 *
 *  Created on: oct 15, 2026
 *      Author: achraf
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "user_interface/shell.h"

int led_init();
int led_control(h_shell_t* h_shell, int argc, char** argv);

#endif /* INC_LED_H_ */
