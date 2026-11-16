/*
 * input_analog.h
 */

#ifndef INC_INPUT_ANALOG_H_
#define INC_INPUT_ANALOG_H_

#include "main.h"
#include "user_interface/shell.h"

void    input_analog_init(void);
int32_t input_analog_get_mA(void);
int     input_analog_cmd(h_shell_t* h_shell, int argc, char** argv);

#endif /* INC_INPUT_ANALOG_H_ */
