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
#include "cmsis_os.h"
#include "Tasks_freertos.h"

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart2;

void stampa_tensioni_celle(Datatocom Data){
	// Stampa risultati misurazione celle
		for (int i = 0; i < N_celle; i++)
		   {
		      char buffer[32];
		      // Converti float in stringa
		      int length = sprintf(buffer, "Valore %d: %.2f V\r\n",i+1, Data.Pack.Cell[i].tensione);
		       // Invia via UART
		      HAL_UART_Transmit_IT(&huart3, (uint8_t*)buffer, length);
		      xSemaphoreTake(UARTSemHandle, osWaitForever);
		   }
}

void stampa_tensioni_GPIO(float *GPIO_voltages){
	// Stampa risultati Misurazione tensione su GPIO
	char buffer[50];
	for (int i = 0; i < 5; i++)
   {
       // Converti float in stringa
       int length = sprintf(buffer, "Valore GPIO %d: %.2f V\r\n",i+1, GPIO_voltages[i]);
       // Invia via UART
       HAL_UART_Transmit_IT(&huart3, (uint8_t*)buffer, length);
       xSemaphoreTake(UARTSemHandle, osWaitForever);
    }
	    // Converti float in stringa
	int length = sprintf(buffer, "Valore Second Reference: %.2f V\r\n", GPIO_voltages[5]);
	    // Invia via UART
	HAL_UART_Transmit_IT(&huart3, (uint8_t*)buffer, length);
	xSemaphoreTake(UARTSemHandle, osWaitForever);
}

void stampa_somma_celle(Status_register reg){
	// Stampa Misurazione somma celle

		char buffer[50];
       // Converti float in stringa
       int length = sprintf(buffer, "La tensione di somma delle celle è: %.2f V\r\n", reg.somma_celle);
       // Invia via UART
       HAL_UART_Transmit_IT(&huart3, (uint8_t*)buffer, length);
       xSemaphoreTake(UARTSemHandle, osWaitForever);
}

void stampa_temperatura_interna(Status_register reg){
	// Stampa Temperatura interna del chip
		char buffer[50];
       // Converti float in stringa
       int length = sprintf(buffer, "La temperatura interna del chip è: %.2f\r\n", reg.int_temperature);
       // Invia via UART
       HAL_UART_Transmit_IT(&huart3, (uint8_t*)buffer, length);
       xSemaphoreTake(UARTSemHandle, osWaitForever);
}

void stampa_SOC(float *SOC){
	// Stampa SOC della prima cella
		char buffer[50];
		for (int i = 0; i < N_celle; i++)
		   {
			// Converti float in stringa
			int length = sprintf(buffer, "Il SOC della cella %d è: %.2f\r\n",i, SOC[i]);
			// Invia via UART
			HAL_UART_Transmit_IT(&huart3, (uint8_t*)buffer, length);
			xSemaphoreTake(UARTSemHandle, osWaitForever);
		   }
}


void debugprint(const char *msg){

	HAL_UART_Transmit_IT(&huart2, (uint8_t*)msg, strlen(msg));
    osSemaphoreAcquire(UARTSemHandle, osWaitForever);

}
