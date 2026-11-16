/*
 * input_encoder.c
 *
 */

#include "acquisition/input_encoder.h"
#include "tim.h"
#include <stdio.h>
#include <string.h>

#define MS_PER_SECOND         1000
#define SEC_PER_MINUTE        60
#define TWO_PI_F              6.2831853f

static uint16_t prev_cnt   = 0;
static int16_t  last_delta = 0;
static int32_t  speed_rpm  = 0;

void input_encoder_init(void)
{
	if (HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL) != HAL_OK) {
		Error_Handler();
	}

	__HAL_TIM_SET_COUNTER(&htim3, 0);
	prev_cnt = 0;

	shell_add(&hshell1, "encoder", input_encoder_cmd, "encoder [zero]");
}

void input_encoder_update(void)
{
	uint16_t curr_cnt = (uint16_t)__HAL_TIM_GET_COUNTER(&htim3);

	last_delta = (int16_t)(curr_cnt - prev_cnt);
	prev_cnt   = curr_cnt;

	speed_rpm = ((int32_t)last_delta * SEC_PER_MINUTE * MS_PER_SECOND)
	            / (COUNTS_PER_REV * SAMPLE_PERIOD_MS);
}

int32_t input_encoder_get_rpm(void)
{
	return speed_rpm;
}

float input_encoder_get_rad_s(void)
{
	return ((float)last_delta * TWO_PI_F * (float)MS_PER_SECOND)
	       / ((float)COUNTS_PER_REV * (float)SAMPLE_PERIOD_MS);
}

int input_encoder_cmd(h_shell_t *h_shell, int argc, char **argv)
{
	if (argc == 2 && strcmp(argv[1], "zero") == 0) {
		__HAL_TIM_SET_COUNTER(&htim3, 0);
		prev_cnt   = 0;
		last_delta = 0;
		speed_rpm  = 0;
	}

	uint16_t raw_cnt = (uint16_t)__HAL_TIM_GET_COUNTER(&htim3);
	int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
	                    "cnt=%u  delta=%d  speed=%ld RPM (%.2f rad/s)\r\n",
	                    raw_cnt, last_delta, (long)speed_rpm,
	                    (double)input_encoder_get_rad_s());
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_OK;
}
