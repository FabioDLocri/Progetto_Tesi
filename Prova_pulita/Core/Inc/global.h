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

extern Cella Batteria[12];

#endif /* INC_GLOBAL_H_ */
