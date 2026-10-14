/*
 * motor.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "motor_control/motor.h"
#include "tim.h"
#include <stdio.h>
#include <stdlib.h>

void motor_init(void)
{
	motor_set_duty(0.6f);   // 60 % for the tests

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

	shell_add(&hshell1, "SETCCR", motor_set_ccr_cmd, "Set PWM CCR: SET_CCR XXXX");
}

void motor_set_ccr(uint32_t ccr)
{
	uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim1) + 1;

	if (ccr > MOTOR_CCR_MAX) ccr = MOTOR_CCR_MAX;
	// max > period
	if (ccr > period)        ccr = period;

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, period - ccr);
}

void motor_set_duty(float alpha)
{
	uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim1) + 1;
	motor_set_ccr((uint32_t)(alpha * period));
}

// Shell command
int motor_set_ccr_cmd(h_shell_t* h_shell, int argc, char** argv)
{
	int size;

	if (argc != 2) {
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
				"Usage : SET_CCR XXXX\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_ERROR;
	}

	char *end;
	long value = strtol(argv[1], &end, 10);

	if (*end != '\0' || end == argv[1] || value < 0) {
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
				"Invalid value : %s\r\n", argv[1]);
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_ERROR;
	}

	uint32_t ccr = (uint32_t)value;
	if (ccr > MOTOR_CCR_MAX) {
		ccr = MOTOR_CCR_MAX;
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
				"CCR limited to %lu\r\n", (unsigned long)MOTOR_CCR_MAX);
		h_shell->drv.transmit(h_shell->print_buffer, size);
	}

	motor_set_ccr(ccr);

	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
			"CCR = %lu\r\n", (unsigned long)ccr);
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_OK;
}
