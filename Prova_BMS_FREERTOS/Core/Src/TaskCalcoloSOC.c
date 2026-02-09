/*
 * TaskCalcoloSOC.c
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */


#include "Tasks_freertos.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include "funzioni_SOC.h"
#include "global.h"
#include "Comunicazione_UART.h"


void TaskCalcoloSOC(void *argument)
{
	Batteria Pacco;
	BaseType_t xreturn;

  	for (;;)
  	{
		xQueueReceive(QueuemisuretoSOC, &Pacco, pdMS_TO_TICKS(2000));
	  // si usa l'AEKF per calcolare SOC e AEKF
		for (int i=0;i<N_celle;i++)
		{
			Pacco.Cell[i].SOC=Pacco.Cell[i].tensione+5/4;
		}
	   	xreturn = xQueueSend(QueueSOCtocom, &Pacco,pdMS_TO_TICKS(200));
	    if (xreturn != pdTRUE)
	    {
	   		debugprint("non scrive nella coda da SOC a com\r\n");
	    }

	    xreturn = xQueueSend(QueueSOCtomain, &Pacco, pdMS_TO_TICKS(200));
	    if (xreturn != pdTRUE)
	    {
	   		debugprint("non scrive nella coda da SOC a main\r\n");
	    }

  }
}

/*
void TaskCalcoloSOC(void *argument)
{
	static uint8_t i=0;
	uint8_t incremento;
	static uint8_t dato=0;
	BaseType_t xreturn;

  for (;;)
  {
	  xQueueReceive(QueuemisuretoSOC, &incremento, pdMS_TO_TICKS(2000));

	  dato += incremento;
	  i++;

	  if (i >= 8)
	  {
		  xreturn = xQueueSend(QueueSOCtocom, &dato,pdMS_TO_TICKS(2000));
	      if (xreturn != pdTRUE)
	      {
	    	  debugprint("non scrive nella coda da SOC a com\r\n");
	      }
	      dato = 0;
	      i = 0;
	  }

  }
}
*/
