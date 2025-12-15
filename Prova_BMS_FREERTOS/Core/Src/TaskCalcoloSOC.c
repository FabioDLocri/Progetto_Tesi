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

/*
void TaskCalcoloSOC(void *argument)
{
	Batteria Pacco[8];
  for (;;)
  {
	  xStreamBufferReceive( BuffertoSOC, (void*) Pacco, sizeof( Pacco[8] ), pdMS_TO_TICKS( 20 ) );
  }
}
*/

void TaskCalcoloSOC(void *argument)
{
	static uint8_t i=0;
	uint8_t incremento;
	static uint8_t dato=0;
	BaseType_t xreturn;

  for (;;)
  {
	  if (xQueueReceive(QueuemisuretoSOC, &incremento, portMAX_DELAY) == pdPASS)
	          {
	              dato += incremento;
	              i++;

	              if (i >= 8)
	              {
	                  xreturn = xQueueSend(QueueSOCtocom, &dato, 0);
	                  if (xreturn != pdTRUE)
	                  {
	                      debugprint("non scrive nella coda da SOC a com\r\n");
	                  }
	                  dato = 0;
	                  i = 0;
	              }
	          }
  }
}
