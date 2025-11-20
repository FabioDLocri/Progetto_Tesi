/*
 * Tasks.h
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */

#ifndef INC_TASKS_H_
#define INC_TASKS_H_

#include "cmsis_os2.h"
#include <stdint.h>


/* Handles/IDs condivisi (opzionali) */
extern osThreadId_t MainTaskHandle;
extern osThreadId_t TaskMisureHandle;
extern osThreadId_t TaskComunicazioneHandle;
extern osThreadId_t TaskCalcoloSOCHandle;


/* Prototipi delle funzioni dei task (CMSIS-RTOS2 signature) */
void MainTask(void *argument);
void TaskMisure(void *argument);
void TaskComunicazione(void *argument);
void TaskCalcoloSOC(void *argument);

/* Funzione helper per creare tutte le risorse e i task */
void StartTasks(void);

#endif /* INC_TASKS_H_ */
