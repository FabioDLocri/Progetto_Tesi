/*
 * TaskComunicazione.c
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

void TaskComunicazione(void *argument)
{


  for (;;)
  {
      /* Aspetta il comando di start */
      stampa_tensioni_celle();
      stampa_tensioni_GPIO(tensione_GPIO);
      stampa_temperatura_interna(int_temperature);
      stampa_somma_celle(somma_celle);
      HAL_Delay(5000);
      /* Segnala al Main che la comunicazione è terminata */
      osThreadSuspend (TaskComunicazione);
  }
}
