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
	uint8_t buffer[8];
	static uint8_t dato=0;
  for (;;)
  {
	  xStreamBufferReceive( BuffertoSOC, (void*) buffer, sizeof( buffer[8] ), pdMS_TO_TICKS( 20 ) );

	  for (int i=0; i<8;i++){
		  dato +=buffer[i];
	  }

	  xQueueSend( QueueSOCtocom, &buffer, 0);
  }
}
