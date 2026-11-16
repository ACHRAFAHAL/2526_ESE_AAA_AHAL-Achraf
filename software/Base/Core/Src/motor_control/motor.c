/*
 * motor.c
 *
 *  Created on: oct 15, 2026
 *      Author: achraf
 */

#include "motor_control/motor.h"
#include "tim.h"
#include <stdio.h>
#include <stdlib.h>

#define PWM_MAX         (__HAL_TIM_GET_AUTORELOAD(&htim1) + 1U)
#define MOTOR_CCR_MIN   (PWM_MAX - MOTOR_CCR_MAX)

#define RAMP_STEP       5U
#define RAMP_PERIOD_MS  10U

static volatile uint8_t  motor_running = 0;
static volatile uint32_t ccr_target;
static volatile uint32_t ccr_current;

static void say(h_shell_t* h, const char* msg)
{
	int size = snprintf(h->print_buffer, SHELL_PRINT_BUFFER_SIZE, "%s\r\n", msg);
	h->drv.transmit(h->print_buffer, size);
}

void motor_init(void)
{
	ccr_current = ccr_target = PWM_MAX / 2;
	motor_set_ccr(ccr_current);            // 50 %

	shell_add(&hshell1, "start",  motor_start_cmd,   "Start PWM at 50 %");
	shell_add(&hshell1, "stop",   motor_stop_cmd,    "Stop PWM");
	shell_add(&hshell1, "speed",  motor_speed_cmd,   "speed XXXX : ramp to CCR XXXX");
	shell_add(&hshell1, "SETCCR", motor_set_ccr_cmd, "Set CCR immediately (bench test)");
}

void motor_set_ccr(uint32_t ccr)
{
	uint32_t period = PWM_MAX;

	if (ccr > MOTOR_CCR_MAX) ccr = MOTOR_CCR_MAX;
	if (ccr < MOTOR_CCR_MIN) ccr = MOTOR_CCR_MIN;

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, period - ccr);
}

void motor_set_duty(float alpha)
{
	motor_set_ccr((uint32_t)(alpha * PWM_MAX));
}

void motor_update(void)
{
	static uint32_t last_tick = 0;

	if (!motor_running) return;

	uint32_t now = HAL_GetTick();
	if ((now - last_tick) < RAMP_PERIOD_MS) return;
	last_tick = now;

	uint32_t cur = ccr_current;
	uint32_t target = ccr_target;
	if (cur == target) return;

	if (cur < target) {
		cur = (target - cur > RAMP_STEP) ? cur + RAMP_STEP : target;
	} else {
		cur = (cur - target > RAMP_STEP) ? cur - RAMP_STEP : target;
	}
	ccr_current = cur;
	motor_set_ccr(cur);
}

int motor_start_cmd(h_shell_t* h_shell, int argc, char** argv)
{
	if (motor_running) { say(h_shell, "Already running"); return HAL_ERROR; }

	ccr_current = ccr_target = PWM_MAX / 2;   // 50 %
	motor_set_ccr(ccr_current);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	motor_running = 1;

	say(h_shell, "PWM started, duty 50 %");
	return HAL_OK;
}

int motor_stop_cmd(h_shell_t* h_shell, int argc, char** argv)
{
	motor_running = 0;

	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);

	ccr_current = ccr_target = PWM_MAX / 2;
	motor_set_ccr(ccr_current);

	say(h_shell, "PWM stopped");
	return HAL_OK;
}

int motor_speed_cmd(h_shell_t* h_shell, int argc, char** argv)
{
	if (argc != 2)      { say(h_shell, "Usage : speed XXXX"); return HAL_ERROR; }
	if (!motor_running) { say(h_shell, "Motor stopped, type start first"); return HAL_ERROR; }

	char *end;
	long value = strtol(argv[1], &end, 10);
	if (*end != '\0' || end == argv[1] || value < 0) {
		say(h_shell, "Invalid value");
		return HAL_ERROR;
	}

	uint32_t ccr = (uint32_t)value;
	if (ccr > MOTOR_CCR_MAX) { ccr = MOTOR_CCR_MAX; say(h_shell, "Limited to max"); }
	if (ccr < MOTOR_CCR_MIN) { ccr = MOTOR_CCR_MIN; say(h_shell, "Limited to min"); }

	ccr_target = ccr;                         // the ramp in motor_update() does the rest

	uint32_t pct10 = (ccr * 1000U) / PWM_MAX; // duty in tenths of a percent (no float printf)
	int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
			"Target CCR = %lu (%lu.%lu %%)\r\n",
			(unsigned long)ccr, (unsigned long)(pct10 / 10), (unsigned long)(pct10 % 10));
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_OK;
}

int motor_set_ccr_cmd(h_shell_t* h_shell, int argc, char** argv)
{
	if (argc != 2) { say(h_shell, "Usage : SETCCR XXXX"); return HAL_ERROR; }

	char *end;
	long value = strtol(argv[1], &end, 10);
	if (*end != '\0' || end == argv[1] || value < 0) {
		say(h_shell, "Invalid value");
		return HAL_ERROR;
	}

	ccr_current = ccr_target = (uint32_t)value;
	motor_set_ccr(ccr_current);
	say(h_shell, "OK");
	return HAL_OK;
}
