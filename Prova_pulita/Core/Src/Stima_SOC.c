/*
 * Stima_SOC.c
 *
 *  Created on: Nov 17, 2025
 *      Author: faxio
 */

#include "main.h"
#include "funzioni_SOC.h"
#include "stm32h7xx_hal.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "global.h"
#include "Config_LTC6811.h"
#include "stdbool.h"

static bool initialized = false;
SoCConfig cfg={
        .capacity_Ah = 20.0f,    // capacità nominale 20 Ah
        .eta_charge = 0.995f,
        .eta_discharge = 1.000f
};

	   // Inizializzo la stima con l'OCV calcolato sopra
void calcolo_SOC(){

	if(!initialized){
	compute_cells_soc_from_voltage (Batteria);

	initialized= true;
	}
	float soc_est[12];
	for (int i = 0;i < 12; i++){
	soc_est[i] = Batteria[i].SOC;
	}

    float dt_s = 0.0001f;
    float alpha = 0.98f;
	// Un passo di aggiornamento ibrido (ripetere a ogni ciclo temporale)
	for (int i = 0; i < 12; ++i) {
	     soc_update_hybrid(&soc_est[i], Batteria[i].tensione, Batteria[i].corrente, dt_s, &cfg, alpha);
	}

}
