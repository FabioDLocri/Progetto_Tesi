/*
 * Comunicazione_UART.h
 *
 *  Created on: Nov 14, 2025
 *      Author: faxio
 */

#ifndef INC_COMUNICAZIONE_UART_H_
#define INC_COMUNICAZIONE_UART_H_

#include "global.h"
#include "string.h"

void stampa_tensioni_celle();

void stampa_tensioni_GPIO(float *GPIO_voltages);

void stampa_temperatura_interna(Status_register reg);

void stampa_somma_celle(Status_register reg);

void stampa_SOC(Batteria*);

void debugprint(const char *msg);

#endif /* INC_COMUNICAZIONE_UART_H_ */
