/*
 * Tasks.c
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */


#include "Tasks.h"
#include "cmsis_os2.h"



osThreadId_t MainTaskHandle=NULL;
osThreadId_t TaskMisureHandle=NULL;
osThreadId_t TaskComunicazioneHandle=NULL;
osThreadId_t TaskCalcoloSOCHandle=NULL;

/* Definitions for BinSem */
osSemaphoreId_t BinSemHandle;
const osSemaphoreAttr_t BinSem_attributes = {
  .name = "BinSem"
};

osSemaphoreId_t SPITXSemHandle;
const osSemaphoreAttr_t SPITXSem_attributes = {
  .name = "SPITXSem"
};

osSemaphoreId_t SPIRXSemHandle;
const osSemaphoreAttr_t SPIRXSem_attributes = {
  .name = "SPIRXSem"
};

osSemaphoreId_t UARTSemHandle;
const osSemaphoreAttr_t UARTSem_attributes = {
  .name = "UARTSem"
};
void StartTasks(void)
{
  const osThreadAttr_t mainTaskAttr = {
    .name = "MainTask",
    .stack_size = 1024,
    .priority = osPriorityNormal
  };

  const osThreadAttr_t misureAttr = {
    .name = "TaskMisure",
    .stack_size = 1024*10,
    .priority = osPriorityNormal1
  };

  const osThreadAttr_t ComunicazioneAttr = {
    .name = "TaskComunicazione",
    .stack_size = 1024,
    .priority = osPriorityNormal1
  };

  const osThreadAttr_t SOCAttr = {
    .name = "TaskCalcoloSOC",
    .stack_size = 1024,
    .priority = osPriorityNormal
  };

  /* creo i thread */
  MainTaskHandle = osThreadNew(MainTask, NULL, &mainTaskAttr);
  TaskMisureHandle = osThreadNew(TaskMisure, NULL, &misureAttr);
  TaskComunicazioneHandle = osThreadNew(TaskComunicazione, NULL, &ComunicazioneAttr);
  TaskCalcoloSOCHandle = osThreadNew(TaskCalcoloSOC, NULL, &SOCAttr);

  //Creo i semafori per i task e le interrupt
  BinSemHandle = osSemaphoreNew(1, 0, &BinSem_attributes);
  SPITXSemHandle = osSemaphoreNew(1, 0, &SPITXSem_attributes);
  SPIRXSemHandle = osSemaphoreNew(1, 0, &SPIRXSem_attributes);
  UARTSemHandle = osSemaphoreNew(1, 0, &UARTSem_attributes);
}

void SPITXCompleteCallback(SPI_HandleTypeDef *spi)
{
      osSemaphoreRelease(SPITXSemHandle);
}
void SPIRXCompleteCallback(SPI_HandleTypeDef *spi)
{
      osSemaphoreRelease(SPIRXSemHandle);
}
void UARTCompleteCallback(UART_HandleTypeDef *huart)
{
      osSemaphoreRelease(UARTSemHandle);
}

