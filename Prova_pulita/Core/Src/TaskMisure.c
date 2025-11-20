/*
 * TaskMisure.c
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */


#include "tasks.h"
#include "stdio.h"
#include <stdint.h>
#include "global.h"
#include "Comunicazione_Stack.h"
#include "Comunicazione_UART.h"

extern UART_HandleTypeDef huart3;

void TaskMisure(void *argument)
{
	ltc6811_configure();

	//verifichiamo se effettivamente stiamo scrivendo nel modo giusto
	HAL_StatusTypeDef status1 = ltc6811_read_data(0x0002, &config[0], 6);
	for (int i = 0; i < 6; i++)
	{
		char buffer2[40];
        	// Converti float in stringa
		int length2 = sprintf(buffer2, "Il valore %d della configurazione: %x \r\n", i+1, config[i]);
      	// Invia via UART
		HAL_UART_Transmit(&huart3, (uint8_t*)buffer2, length2, HAL_MAX_DELAY);
	}

  for (;;){
	  ltc6811_read_cell_voltages();
	  ltc6811_read_gpio_voltages(tensione_GPIO);
	  ltc6811_read_status();
	  osDelay(100);
      osThreadResume(TaskCalcoloSOC);

      osThreadSuspend (TaskMisure);
  }
}
