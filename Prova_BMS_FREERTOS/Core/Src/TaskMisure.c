/*
 * TaskMisure.c
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */


#include "Tasks_freertos.h"
#include "stdio.h"
#include <stdint.h>
#include "global.h"
#include "Comunicazione_Stack.h"
#include "Comunicazione_UART.h"
#include "FreeRTOS.h"

extern UART_HandleTypeDef huart3;

void TaskMisure(void *argument)
{
	HAL_StatusTypeDef Status_read;
	TickType_t xLastExecutionTime = xTaskGetTickCount();
	Batteria Pacco_bat;
	float tensione_GPIO[6]={0};
	Status_register Status;
	Datatocom Misura;
	BaseType_t xreturn;


	ltc6811_configure();

	for (;;){
		Status_read=ltc6811_read_cell_voltages(&Pacco_bat);
		ltc6811_read_gpio_voltages(tensione_GPIO);
		ltc6811_read_status(&Pacco_bat, &Status);

		Misura.Pack=Pacco_bat;
		Misura.reg=Status;

		xreturn = xQueueSend( Queuemisuretomain, &Pacco_bat, pdMS_TO_TICKS(200) );
		if (xreturn!=pdTRUE){
			debugprint("non scrive nella coda da misure a main");
		}

		xreturn = xQueueSend( Queuemisuretocom, &Misura,pdMS_TO_TICKS(200) );
		if (xreturn!=pdTRUE){
			debugprint("non scrive nella coda da misure a com\n\r");
		}

		xreturn = xQueueSend( QueuemisuretoSOC, &Pacco_bat, pdMS_TO_TICKS(200) );
		if (xreturn!=pdTRUE){
			debugprint("non scrive nella coda da misure a SOC\n\r");
		}

		vTaskDelayUntil(&xLastExecutionTime, pdMS_TO_TICKS(1000));
  }
}

/*
void TaskMisure(void *argument)
{
	TickType_t xLastExecutionTime = xTaskGetTickCount();
	uint8_t numero=0;
	BaseType_t xreturn;

	for (;;){
		if(numero==16){
			numero=0;
		}
		numero=numero+1;

		xreturn = xQueueSend( Queuemisuretomain, &numero, 0 );
		if (xreturn!=pdTRUE){
			debugprint("non scrive nella coda da misure a main");
		}
		xreturn = xQueueSend( Queuemisuretocom, &numero,0 );
		if (xreturn!=pdTRUE){
			debugprint("non scrive nella coda da misure a com\n\r");
		}
		xreturn = xQueueSend( QueuemisuretoSOC, &numero,0 );
		if (xreturn!=pdTRUE){
			debugprint("non scrive nella coda da misure a SOC\n\r");
		}

		vTaskDelayUntil(&xLastExecutionTime, pdMS_TO_TICKS(1000));
  }
}*/
