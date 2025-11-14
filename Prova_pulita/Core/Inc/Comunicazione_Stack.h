/*
 * Comunicazione_Stack.h
 *
 *  Created on: Nov 11, 2025
 *      Author: faxio
 */

#ifndef INC_COMUNICAZIONE_STACK_H_
#define INC_COMUNICAZIONE_STACK_H_

#include "main.h"
#include "stm32h7xx_hal.h"

uint16_t calculate_pec(uint8_t *data, uint8_t len);

HAL_StatusTypeDef ltc6811_send_command(uint16_t command);

HAL_StatusTypeDef ltc6811_write_data(uint16_t command, uint8_t *data, uint8_t data_len);

HAL_StatusTypeDef ltc6811_read_data(uint16_t command,uint8_t *rx_data, uint8_t data_len);

HAL_StatusTypeDef ltc6811_configure();

HAL_StatusTypeDef ltc6811_read_cell_voltages(float *voltages);

HAL_StatusTypeDef ltc6811_read_status_a(float *somma_celle, float *int_temperature, float *analog_power_supply);

HAL_StatusTypeDef ltc6811_read_gpio_voltages(float *gpio_voltage);


#endif /* INC_COMUNICAZIONE_STACK_H_ */
