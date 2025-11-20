/*
 * global.h
 *
 *  Created on: Nov 16, 2025
 *      Author: faxio
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "main.h"
#include "stdbool.h"


extern uint8_t timer_flag;

extern uint8_t config[6];
extern uint8_t configuration_data[6];

extern float tensione_GPIO[6];

extern float somma_celle;
extern float int_temperature;
extern float analog_power_supply;
extern float digital_power_supply;

typedef struct {
	float tensione;
	float corrente;
	float temperatura;
	float SOC;
	float SOH;
	bool COV;// Cell x 	Overvoltage Flag
	bool CUV;// Cell x 	Undervoltage Flag
}Cella;

// Configurazione per il calcolo SoC
typedef struct {
    // Capacità cella in Ah (necessaria per il coulomb counting)
    float capacity_Ah;

    // Efficienze per correzione coulomb counting
    float eta_charge;     // es: 0.995 (perdita in carica)
    float eta_discharge;  // es: 1.000 o 0.999
} SoCConfig;

extern Cella Batteria[12];

extern SoCConfig cfg;

#endif /* INC_GLOBAL_H_ */
