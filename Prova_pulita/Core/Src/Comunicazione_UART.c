/*
 * Comunicazione_UART.c
 *
 *  Created on: Nov 14, 2025
 *      Author: faxio
 */



#include "Comunicazione_UART.h"
#include "main.h"
#include "stm32h7xx_hal.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "global.h"


extern UART_HandleTypeDef huart3;


void stampa_tensioni_celle(){
	// Stampa risultati misurazione celle
		for (int i = 0; i < 12; i++)
		   {
		      char buffer[32];
		      // Converti float in stringa
		      int length = sprintf(buffer, "Valore %d: %.2f V\r\n",i+1, Batteria[i].tensione);
		       // Invia via UART
		      HAL_UART_Transmit(&huart3, (uint8_t*)buffer, length, HAL_MAX_DELAY);
		   }
}

void stampa_tensioni_GPIO(float *GPIO_voltages){
	// Stampa risultati
	char buffer[32];
	for (int i = 0; i < 5; i++)
   {
       // Converti float in stringa
       int length = sprintf(buffer, "Valore GPIO %d: %.2f V\r\n",i+1, GPIO_voltages[i]);
       // Invia via UART
       HAL_UART_Transmit(&huart3, (uint8_t*)buffer, length, HAL_MAX_DELAY);
    }
	    // Converti float in stringa
	int length = sprintf(buffer, "Valore Second Reference: %.2f V\r\n", GPIO_voltages[5]);
	    // Invia via UART
	HAL_UART_Transmit(&huart3, (uint8_t*)buffer, length, HAL_MAX_DELAY);
}

void stampa_somma_celle(float somma_celle){
	// Stampa risultati

		char buffer[50];
       // Converti float in stringa
       int length = sprintf(buffer, "La tensione di somma delle celle è: %.2f V\r\n", somma_celle);
       // Invia via UART
       HAL_UART_Transmit(&huart3, (uint8_t*)buffer, length, HAL_MAX_DELAY);
}

void stampa_temperatura_interna(float temperatura){
	// Stampa risultati
		char buffer[50];
       // Converti float in stringa
       int length = sprintf(buffer, "La temperatura interna del chip è: %.2f\r\n", temperatura);
       // Invia via UART
       HAL_UART_Transmit(&huart3, (uint8_t*)buffer, length, HAL_MAX_DELAY);
}

