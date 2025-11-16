/*
 * Comunicazione_ADC.h
 *
 *  Created on: Nov 14, 2025
 *      Author: faxio
 */

#ifndef INC_COMUNICAZIONE_ADC_H_
#define INC_COMUNICAZIONE_ADC_H_

#include "main.h"
#include "stm32h7xx_hal.h"

void MAX1227_Init(void);

void MAX1227_WriteByte(uint8_t data);

float* read_all_voltages(void);

float read_single_channel(uint8_t channel);

float read_shunt_voltage(void);

float read_voltage_1(void);
float read_voltage_2(void);
float read_voltage_3(void);
float read_voltage_4(void);

#endif /* INC_COMUNICAZIONE_ADC_H_ */
