/*
 * analog_input.c
 *
 */

#include "acquisition/input_analog.h"
#include "adc.h"
#include <stdio.h>
#include <string.h>

#define ADC_VREF_MV        3300
#define ADC_FULL_SCALE     4095
#define SENSOR_SENS_MV_A   100
#define SENSOR_GAIN_X1000  1000

#define ADC_BUF_LEN 2

static uint16_t          adc_buf[ADC_BUF_LEN];
static volatile uint32_t filt16;
static volatile uint32_t sample_count;
static volatile int32_t  offset_mV = ADC_VREF_MV / 2;

static inline void new_sample(uint16_t raw)
{
	filt16 += raw - (filt16 >> 4);
	sample_count++;
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == ADC1) new_sample(adc_buf[0]);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == ADC1) new_sample(adc_buf[1]);
}

void input_analog_init(void)
{	ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Init.NbrOfConversion = 1;
    HAL_ADC_Init(&hadc1);

    	sConfig.Channel      = ADC_CHANNEL_2;
    	sConfig.Rank         = ADC_REGULAR_RANK_1;
    	sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
    	sConfig.SingleDiff   = ADC_SINGLE_ENDED;
    	sConfig.OffsetNumber = ADC_OFFSET_NONE;
    	sConfig.Offset       = 0;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, ADC_BUF_LEN);
	shell_add(&hshell1, "current", input_analog_cmd, "current [zero]");
}

static int32_t raw_to_mV(uint32_t raw)
{
	return (int32_t)((raw * ADC_VREF_MV) / ADC_FULL_SCALE);
}

int32_t input_analog_get_mA(void)
{
	int32_t v_mV = raw_to_mV(filt16 >> 4) * 1000 / SENSOR_GAIN_X1000;
	return ((v_mV - offset_mV) * 1000) / SENSOR_SENS_MV_A;
}

int input_analog_cmd(h_shell_t* h_shell, int argc, char** argv)
{
	uint32_t raw = filt16 >> 4;

	if (argc == 2 && strcmp(argv[1], "zero") == 0) {
		offset_mV = raw_to_mV(raw) * 1000 / SENSOR_GAIN_X1000;
	}

	int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
			"raw=%lu  V=%ld mV  I=%ld mA  offset=%ld mV  n=%lu\r\n",
			(unsigned long)raw, (long)raw_to_mV(raw), (long)input_analog_get_mA(),
			(long)offset_mV, (unsigned long)sample_count);
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_OK;
}
