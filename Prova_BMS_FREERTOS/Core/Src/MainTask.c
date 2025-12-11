/*
 * MainTask.c
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */

#include "Tasks_freertos.h"
#include <stdio.h>
#include "Comunicazione_Stack.h"
#include "Comunicazione_UART.h"
#include "global.h"
#include "funzioni_SOC.h"
#include "Manage_bms.h"
#include "FreeRTOS.h"

void MainTask(void *argument)
{
	QueueSetMemberHandle_t xHandle;

  for (;;)
  {
	  xHandle = xQueueSelectFromSet( Settomain, pdMS_TO_TICKS(100) );
	  Batteria Pacco_bat;
	  char stringa[50];


	  if( xHandle == NULL )
	       {
		  	  debugprint("è passato troppo tempo dalla richiesta\n");
	       }
	       else if( xHandle == ( QueueSetMemberHandle_t ) Queuemisuretomain )
	       {
	          xQueueReceive( Queuemisuretomain,(void *)&Pacco_bat, 0 );

	       }
	       else if( xHandle == ( QueueSetMemberHandle_t ) Queueuarttomain )
	       {
	          xQueueReceive(Queueuarttomain,(void *) &stringa, 0 );
	       }

      osDelay(1);
  }
}
