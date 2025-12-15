/*
 * Tasks_freertos.h
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */

#ifndef INC_TASKS_FREERTOS_H_
#define INC_TASKS_FREERTOS_H_

#include "cmsis_os2.h"
#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "stream_buffer.h"

extern SemaphoreHandle_t SPITXSemHandle;
extern SemaphoreHandle_t SPIRXSemHandle;
extern SemaphoreHandle_t UARTSemHandle;

extern QueueHandle_t Queuemisuretomain;
extern QueueHandle_t Queuemisuretocom;
extern QueueHandle_t Queueuarttomain;
extern QueueHandle_t QueueSOCtocom;
extern QueueHandle_t QueuemisuretoSOC;


extern QueueSetHandle_t Settomain;
extern QueueSetHandle_t Settocom;

/* Prototipi delle funzioni dei task  */
void MainTask(void *argument);
void TaskMisure(void *argument);
void TaskComunicazione(void *argument);
void TaskCalcoloSOC(void *argument);

/* Funzione helper per creare tutte le risorse e i task */
void StartTasks(void);

void SPITXCompleteCallback(SPI_HandleTypeDef *spi);
void SPIRXCompleteCallback(SPI_HandleTypeDef *spi);
void UARTCompleteCallback(UART_HandleTypeDef *huart);
void UARTRXCompleteCallback(UART_HandleTypeDef *huart);

#endif /* INC_TASKS_FREERTOS_H_ */
