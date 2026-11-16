/*
 * motor.h
 *
 *  Created on: oct 15, 2026
 *      Author: achraf
 */

#ifndef INC_MOTOR_CONTROL_MOTOR_H_
#define INC_MOTOR_CONTROL_MOTOR_H_

#include "main.h"
#include "user_interface/shell.h"

// Maximum CCR
#define MOTOR_CCR_MAX   3825U     // 90% of 4250

void motor_init(void);
void motor_set_duty(float alpha);
void motor_set_ccr(uint32_t ccr);
void motor_update(void);

int  motor_set_ccr_cmd(h_shell_t* h_shell, int argc, char** argv);
int motor_start_cmd(h_shell_t* h_shell, int argc, char** argv);
int motor_stop_cmd(h_shell_t* h_shell, int argc, char** argv);
int motor_speed_cmd(h_shell_t* h_shell, int argc, char** argv);

#endif /* INC_MOTOR_CONTROL_MOTOR_H_ */
