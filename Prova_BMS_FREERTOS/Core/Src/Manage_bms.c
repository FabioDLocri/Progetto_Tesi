/*
 * Manage_bms.c
 *
 *  Created on: Nov 20, 2025
 *      Author: faxio
 */
/*


#include "global.h"
#include "stdbool.h"

void Manage_bms(void){

	void ledredoff();
	void ledyellowoff();
	void ledgreenoff();

    //---Controllo sulla corrente---


	if(Batteria[1].corrente<0){

		//accendi un led per segnalare la scarica
	    //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

	}else if (Batteria[1].corrente>0.0f){

		//accendi un led per segnalare la scarica
		void ledredon();

	}else {

		//accendi un led per segnalare che non è né in carica né in scarica

	}

	//---Controllo sulle tensioni---

	bool alarm_present=false;

	for (int i=0; i<12; i++){
		if(Batteria[i].COV==true||Batteria[i].CUV==true){
			alarm_present=true;
		}
	}

	if(alarm_present){
		void ledyellowon();
	}

}
*/
