/*
 * TaskComunicazione.c
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */


#include "Tasks_freertos.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include "Comunicazione_Stack.h"
#include "Comunicazione_UART.h"
#include "global.h"
#include "funzioni_SOC.h"

void TaskComunicazione(void *argument)
{
	Datatocom Misure;
	QueueSetMemberHandle_t xHandle;
	float *SOC;
  for (;;)
  {
	  xHandle = xQueueSelectFromSet( Settocom, pdMS_TO_TICKS(200) );

	  if( xHandle == NULL )
	       {
		  	  debugprint("è passato troppo tempo dalla richiesta\n");
	       }
	       else if( xHandle == ( QueueSetMemberHandle_t ) Queuemisuretocom )
	       {
	          xQueueReceive( Queuemisuretocom, &Misure, 0 );
	          stampa_tensioni_celle(Misure);
	          stampa_temperatura_interna(Misure.reg);
	          stampa_somma_celle(Misure.reg);
	       }
	       else if( xHandle == ( QueueSetMemberHandle_t ) QueueSOCtocom )
	       {
	          xQueueReceive(QueueSOCtocom, &SOC, 0 );
	          stampa_SOC(SOC);
	       }

  }
}
