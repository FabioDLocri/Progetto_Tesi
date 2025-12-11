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

void TaskCalcoloSOC(void *argument)
{
	Batteria Pacco[8];
  for (;;)
  {
	  xStreamBufferReceive( BuffertoSOC, ( void * ) Pacco,sizeof( Pacco[8] ), pdMS_TO_TICKS( 20 ) );
  }
}
