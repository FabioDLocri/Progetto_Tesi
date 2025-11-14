/*
 * Comunicazione_UART.h
 *
 *  Created on: Nov 14, 2025
 *      Author: faxio
 */

#ifndef INC_COMUNICAZIONE_UART_H_
#define INC_COMUNICAZIONE_UART_H_

void stampa_tensioni_celle(float *cell_voltages);

void stampa_tensioni_GPIO(float *GPIO_voltages);

void stampa_temperatura_interna(float temp);

void stampa_somma_celle(float somma_celle);

#endif /* INC_COMUNICAZIONE_UART_H_ */
