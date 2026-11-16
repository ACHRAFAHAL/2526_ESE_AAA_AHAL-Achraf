/*
 * input_encoder.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_ACQUISITION_INPUT_ENCODER_H_
#define INC_ACQUISITION_INPUT_ENCODER_H_

#include "main.h"
#include "user_interface/shell.h"
#include <stdint.h>

#define ENCODER_PPR           1024
#define QUADRATURE_FACTOR     4
#define COUNTS_PER_REV        (ENCODER_PPR * QUADRATURE_FACTOR)
#define SAMPLE_PERIOD_MS      10

extern h_shell_t hshell1;

void    input_encoder_init(void);
void    input_encoder_update(void);
int32_t input_encoder_get_rpm(void);
float   input_encoder_get_rad_s(void);
int     input_encoder_cmd(h_shell_t *h_shell, int argc, char **argv);

#endif /* INC_ACQUISITION_INPUT_ENCODER_H_ */
