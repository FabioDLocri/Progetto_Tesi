/*
 * TaskMisure.c
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */


#include "Tasks.h"
#include "stdio.h"
#include <stdint.h>
#include "global.h"
#include "Comunicazione_Stack.h"
#include "Comunicazione_UART.h"

extern UART_HandleTypeDef huart3;

void TaskMisure(void *argument)
{
	osSemaphoreAcquire(BinSemHandle,osWaitForever);
	ltc6811_configure();


  for (;;){
	  ltc6811_read_cell_voltages();
	  ltc6811_read_gpio_voltages(tensione_GPIO);
	  ltc6811_read_status();
      osThreadResume(TaskCalcoloSOCHandle);

      osSemaphoreRelease(BinSemHandle);
      osThreadSuspend (TaskMisureHandle);
  }
}
