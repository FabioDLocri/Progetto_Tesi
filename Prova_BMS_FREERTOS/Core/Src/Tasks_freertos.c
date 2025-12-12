/*
 * Tasks_freertos.c
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */


#include "Tasks_freertos.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "global.h"
#include "Comunicazione_UART.h"

SemaphoreHandle_t SPITXSemHandle;
SemaphoreHandle_t SPIRXSemHandle;
SemaphoreHandle_t UARTSemHandle;


QueueHandle_t Queuemisuretomain;
QueueHandle_t Queuemisuretocom;
QueueHandle_t Queueuarttomain;
 QueueHandle_t QueueSOCtocom;

StreamBufferHandle_t BuffertoSOC;

QueueSetHandle_t Settomain;
QueueSetHandle_t Settocom;

static uint8_t uartbuffer;
extern UART_HandleTypeDef huart3;
BaseType_t xreturn;

void StartTasks(void)
{
	xTaskCreate(MainTask, "MainTask",1024, NULL, 24, NULL);
	xTaskCreate(TaskMisure, "TaskMisure",1024 , NULL, 25, NULL);
	xTaskCreate(TaskComunicazione, "TaskComunicazione",1024, NULL, 23, NULL);
	xTaskCreate(TaskCalcoloSOC, "TaskCalcoloSOC",1024, NULL, 22, NULL);


	//Creo i semafori per le interrupt
	SPITXSemHandle = xSemaphoreCreateBinary();
	SPIRXSemHandle = xSemaphoreCreateBinary();
	UARTSemHandle = xSemaphoreCreateBinary();

/*	Queuemisuretomain = xQueueCreate(8,sizeof(Batteria));
	Queuemisuretocom = xQueueCreate(8,sizeof(Datatocom));
	Queueuarttomain = xQueueCreate(2,sizeof(uint8_t));
	QueueSOCtocom = xQueueCreate(8,sizeof(float[N_celle]));

	BuffertoSOC = xStreamBufferCreate(24*sizeof(Batteria),8*sizeof(Batteria));
*/

	//creati per provare il progetto
	Queuemisuretomain = xQueueCreate(8,sizeof(uint8_t));
	Queuemisuretocom = xQueueCreate(8,sizeof(uint8_t));
	Queueuarttomain = xQueueCreate(2,sizeof(uint8_t));
	QueueSOCtocom = xQueueCreate(8,sizeof(uint8_t));
	BuffertoSOC = xStreamBufferCreate(24*sizeof(uint8_t),8*sizeof(uint8_t));


	xreturn = xQueueAddToSet( Queuemisuretocom, Settocom );
	if (xreturn!=pdPASS){
		debugprint("la coda non è inserita nel settocom");
	}
	xreturn = xQueueAddToSet( QueueSOCtocom, Settocom );
	if (xreturn!=pdPASS){
		debugprint("la coda non è inserita nel settocom");
	}

	xreturn = xQueueAddToSet( Queuemisuretomain, Settomain );
	if (xreturn!=pdPASS){
		debugprint("la coda non è inserita nel settomain");
	}
	xreturn = xQueueAddToSet( Queueuarttomain, Settomain );
	if (xreturn!=pdPASS){
		debugprint("la coda non è inserita nel settomain");
	}

	HAL_UART_Receive_IT(&huart3, &uartbuffer, 1);
}

void SPITXCompleteCallback(SPI_HandleTypeDef *spi)
{
	xSemaphoreGive(SPITXSemHandle);
}
void SPIRXCompleteCallback(SPI_HandleTypeDef *spi)
{
	xSemaphoreGive(SPIRXSemHandle);
}
void UARTCompleteCallback(UART_HandleTypeDef *huart)
{
	xSemaphoreGive(UARTSemHandle);
}

void UARTRXCompleteCallback(UART_HandleTypeDef *huart)
{
	xQueueSendToBackFromISR(Queueuarttomain,uartbuffer,1 );
	HAL_UART_Receive_IT(&huart3,&uartbuffer,1);
}
