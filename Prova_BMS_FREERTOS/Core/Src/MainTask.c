/*
 * MainTask.c
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */

#include "Tasks_freertos.h"
#include <stdio.h>
#include "Comunicazione_Stack.h"
#include "Comunicazione_UART.h"
#include "global.h"
#include "funzioni_SOC.h"
#include "Manage_bms.h"
#include "FreeRTOS.h"
#include "string.h"
/*
void MainTask(void *argument)
{
	QueueSetMemberHandle_t xHandle;
	Batteria Pacco_bat;
	xHandle = xQueueSelectFromSet( Settomain, pdMS_TO_TICKS(100) );
	char stringa[50] ={0};
	uint8_t i=0;
	BaseType_t xreturn;
	static char carattere=0;

  for (;;)
  {

	xHandle = xQueueSelectFromSet( Settomain, pdMS_TO_TICKS(2000) );
	  if( xHandle == ( QueueSetMemberHandle_t ) Queuemisuretomain )
	       {
	          xQueueReceive( Queuemisuretomain,(void *)&Pacco_bat, pdMS_TO_TICKS(500) );
	       }
	       else if( xHandle == ( QueueSetMemberHandle_t ) Queueuarttomain )
	       {

	    	   xreturn= xQueueReceive(Queueuarttomain,(void *) &carattere, pdMS_TO_TICKS(2000) );

	    	   if (xreturn!=pdTRUE){
	    		   debugprint("non ricevo dalla coda\n\r");
	    	   }
	    	   if(carattere != '\n' && carattere != '\r')
	    	   {
	    		   stringa[i] = carattere;
	    		   i++;
	    	   }
	    	   else
	    	   {
	   			// Aggiungo il carattere nullo per terminare la stringa
	    		   stringa[i] = '\0';
	    	    // Confronta la stringa ricevuta con "ciao"
	    	  	   if(strcmp(stringa, "ciao") == 0) {
	    	  		   ledyellowon(); // Accendi il LED
	    	   	   }
	    	  	   else
	    	   	   {
	    	  		   ledyellowoff();
	    	   	   }
	    	  	   if(strcmp(stringa, "pippo") == 0) {
	    	  		   ledgreenon(); // Accendi il LED
	    	   	   }
	    	  	   else
	    	   	   {
	    	  		   ledgreenoff();
	    	   	   }
	    	   		 // Resetta l'indice per la prossima stringa
	    	   	   i = 0;
	    	   }
	       }
  }
}
*/

void MainTask(void *argument)
{
		QueueSetMemberHandle_t xHandle;

		uint8_t incremento;
		static char carattere=0;
		char stringa[50]={0};
		uint8_t i=0;
		BaseType_t xreturn;
  for (;;)
  {
	  xHandle = xQueueSelectFromSet( Settomain, pdMS_TO_TICKS(2000) );
	  if( xHandle == ( QueueSetMemberHandle_t ) Queuemisuretomain )
	       {
	          xQueueReceive( Queuemisuretomain,(void *)&incremento, pdMS_TO_TICKS(2000) );

	          if ((incremento%2)==1){
	        	  ledredon();
	          }
	          else  ledredoff();

	       }
	       else if( xHandle == ( QueueSetMemberHandle_t ) Queueuarttomain )
	       {

	    	   xreturn= xQueueReceive(Queueuarttomain,(void *) &carattere, pdMS_TO_TICKS(2000) );
	    	   if (xreturn!=pdTRUE){
	    		   debugprint("non ricevo dalla coda\n\r");
	    	   }
	    	   if(carattere != '\n' && carattere != '\r')
	    	   {
	    		   stringa[i] = carattere;
	    		   i++;
	    	   }
	    	   else
	    	   {
	   			// Aggiungi il carattere nullo per terminare la stringa
	    		   stringa[i] = '\0';
	    	    // Confronta la stringa ricevuta con "ciao"
	    	  	   if(strcmp(stringa, "ciao") == 0) {
	    	  		   ledyellowon(); // Accendi il LED
	    	   	   }
	    	  	   else
	    	   	   {
	    	  		   ledyellowoff();
	    	   	   }
	    	  	   if(strcmp(stringa, "pippo") == 0) {
	    	  		   ledgreenon(); // Accendi il LED
	    	   	   }
	    	  	   else
	    	   	   {
	    	  		   ledgreenoff();
	    	   	   }
	    	   		 // Resetta l'indice per la prossima stringa
	    	   	   i = 0;
	    	   }
	       }
  }
}
