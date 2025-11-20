/*
 * MainTask.c
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */

#include "Tasks.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include "Comunicazione_Stack.h"
#include "Comunicazione_UART.h"
#include "Comunicazione_ADC.h"
#include "global.h"
#include "funzioni_SOC.h"
#include "Manage_bms.h"

void MainTask(void *argument)
{


  for (;;)
  {
      osThreadResume(TaskMisureHandle);
      osSemaphoreRelease(BinSemHandle);

      Manage_bms();

      osThreadResume(TaskComunicazioneHandle);
      osSemaphoreRelease(BinSemHandle);

      osDelay(1);
  }
}
