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
QueueHandle_t QueuemisuretoSOC;

StreamBufferHandle_t BuffertoSOC;

QueueSetHandle_t Settomain;
QueueSetHandle_t Settocom;

static uint8_t uartbuffer;
extern UART_HandleTypeDef huart3;
BaseType_t xreturn;

void StartTasks(void)
{
	xreturn=xTaskCreate(MainTask, "MainTask",128, NULL, 25, NULL);
	if (xreturn!=pdPASS){
		debugprint("Task1 non creato correttamente\r\n");
	}
	xreturn=xTaskCreate(TaskMisure, "TaskMisure",1024 , NULL, 26, NULL);
	if (xreturn!=pdPASS){
		debugprint("Task2 non creato correttamente\r\n");
	}
	xreturn=xTaskCreate(TaskComunicazione, "TaskComunicazione",512, NULL, 24, NULL);
	if (xreturn!=pdPASS){
		debugprint("Task3 non creato correttamente\r\n");
	}
	xreturn = xTaskCreate(TaskCalcoloSOC, "TaskCalcoloSOC",512, NULL, 23, NULL);
	if (xreturn!=pdPASS){
		debugprint("Task4 non creato correttamente\r\n");
	}

	//Creo i semafori per le interrupt
	SPITXSemHandle = xSemaphoreCreateBinary();
	SPIRXSemHandle = xSemaphoreCreateBinary();
	UARTSemHandle = xSemaphoreCreateBinary();

/*	Queuemisuretomain = xQueueCreate(8,sizeof(Batteria));
	Queuemisuretocom = xQueueCreate(8,sizeof(Datatocom));
	Queueuarttomain = xQueueCreate(8,sizeof(uint8_t));
	QueueSOCtocom = xQueueCreate(8,sizeof(float[N_celle]));

	QueuemisuretoSOC = xQueueCreate(8,sizeof(uint8_t));

*/

	//creati per provare il progetto
	Queuemisuretomain = xQueueCreate(8,sizeof(uint8_t));
	Queuemisuretocom = xQueueCreate(8,sizeof(uint8_t));
	Queueuarttomain = xQueueCreate(8,sizeof(uint8_t));
	QueueSOCtocom = xQueueCreate(8,sizeof(uint8_t));

	QueuemisuretoSOC = xQueueCreate(8,sizeof(uint8_t));

    Settocom = xQueueCreateSet( 8 + 8 );
	if (Settocom == NULL) {
		debugprint("Errore creazione Settocom\n");
	}
    Settomain = xQueueCreateSet( 8 + 2 );
	if (Settomain == NULL) {
		debugprint("Errore creazione Settomain\n");
	}

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
	xQueueSendToBackFromISR(Queueuarttomain,&uartbuffer,NULL);
	HAL_UART_Receive_IT(&huart3,&uartbuffer,1);
}
