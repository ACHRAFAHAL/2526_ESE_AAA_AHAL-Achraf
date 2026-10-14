/*
 * motor.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_MOTOR_CONTROL_MOTOR_H_
#define INC_MOTOR_CONTROL_MOTOR_H_

#include "main.h"
#include "user_interface/shell.h"

// Maximum CCR allowed (TO BE SET from the cahier des charges)
#define MOTOR_CCR_MAX   3825U     // placeholder: 90 % of 4250

void motor_init(void);
void motor_set_duty(float alpha);
void motor_set_ccr(uint32_t ccr);
int  motor_set_ccr_cmd(h_shell_t* h_shell, int argc, char** argv);

#endif /* INC_MOTOR_CONTROL_MOTOR_H_ */
