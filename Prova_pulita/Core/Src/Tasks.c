/*
 * Tasks.c
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */


#include "tasks.h"
#include "cmsis_os2.h"



osThreadId_t MainTaskHandle = NULL;
osThreadId_t TaskMisureHandle = NULL;
osThreadId_t TaskComunicazioneHandle = NULL;
osThreadId_t TaskCalcoloSOCHandle = NULL;

void StartTasks(void)
{
  const osThreadAttr_t mainTaskAttr = {
    .name = "MainTask",
    .stack_size = 1024,
    .priority = osPriorityNormal
  };

  const osThreadAttr_t misureAttr = {
    .name = "TaskMisure",
    .stack_size = 1024,
    .priority = osPriorityNormal1
  };

  const osThreadAttr_t ComunicazioneAttr = {
    .name = "TaskComunicazione",
    .stack_size = 1024,
    .priority = osPriorityNormal1
  };

  const osThreadAttr_t SOCAttr = {
    .name = "TaskCalcoloCOC",
    .stack_size = 1024,
    .priority = osPriorityNormal
  };

  /* crea i thread */
  MainTaskHandle = osThreadNew(MainTask, NULL, &mainTaskAttr);
  TaskMisureHandle = osThreadNew(TaskMisure, NULL, &misureAttr);
  TaskComunicazioneHandle = osThreadNew(TaskComunicazione, NULL, &ComunicazioneAttr);
  TaskCalcoloSOCHandle = osThreadNew(TaskCalcoloSOC, NULL, &SOCAttr);

}
