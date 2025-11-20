/*
 * TaskCalcoloSOC.c
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */


#include "Tasks.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include "funzioni_SOC.h"
#include "global.h"

void TaskCalcoloSOC(void *argument)
{

  for (;;)
  {
	  calcolo_SOC();
      osThreadSuspend (TaskCalcoloSOCHandle);
  }
}
