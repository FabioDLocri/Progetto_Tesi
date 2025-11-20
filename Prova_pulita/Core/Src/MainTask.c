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

void MainTask(void *argument)
{


  for (;;)
  {
      osThreadResume(TaskMisure);
      /* 1) Start misure */
 //     osThreadFlagsSet(TaskMisure, FLAG_MISURE_START);
      osThreadResume(TaskComunicazione);
      /* 2) Aspetta che TaskMisure segnali il completamento */
 //     osThreadFlagsWait(FLAG_MISURE_DONE, osFlagsWaitAny, osWaitForever);
      /* osThreadFlagsWait rimuove automaticamente la flag che ha soddisfatto la condizione */

      /* 3) Start comunicazione */
 //     osThreadFlagsSet(TaskComunicazione, FLAG_COMM_START);

      /* 4) Aspetta che TaskComunicazione segnali il completamento */
 //     osThreadFlagsWait(FLAG_COMM_DONE, osFlagsWaitAny, osWaitForever);

      /* Piccola pausa per evitare busy loop */
      osDelay(1);
  }
}
