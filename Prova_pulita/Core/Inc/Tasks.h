/*
 * Tasks.h
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */

#ifndef INC_TASKS_H_
#define INC_TASKS_H_

#include "cmsis_os2.h"
#include <stdint.h>
#include "stm32h7xx_hal.h"


extern osThreadId_t MainTaskHandle;
extern osThreadId_t TaskMisureHandle;
extern osThreadId_t TaskComunicazioneHandle;
extern osThreadId_t TaskCalcoloSOCHandle;

extern osSemaphoreId_t BinSemHandle;
extern osSemaphoreId_t SPITXSemHandle;
extern osSemaphoreId_t SPIRXSemHandle;
extern osSemaphoreId_t UARTSemHandle;

/* Prototipi delle funzioni dei task (CMSIS-RTOS2 signature) */
void MainTask(void *argument);
void TaskMisure(void *argument);
void TaskComunicazione(void *argument);
void TaskCalcoloSOC(void *argument);

/* Funzione helper per creare tutte le risorse e i task */
void StartTasks(void);

void SPITXCompleteCallback(SPI_HandleTypeDef *spi);
void SPIRXCompleteCallback(SPI_HandleTypeDef *spi);
void UARTCompleteCallback(UART_HandleTypeDef *huart);

#endif /* INC_TASKS_H_ */
