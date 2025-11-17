/*
 * global.h
 *
 *  Created on: Nov 16, 2025
 *      Author: faxio
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "main.h"


extern uint8_t timer_flag;

extern uint8_t config[6];
extern uint8_t configuration_data[6];

extern float tensione_celle[12];
extern float tensione_GPIO[6];

extern float somma_celle;
extern float int_temperature;
extern float analog_power_supply;
extern float digital_power_supply;

struct Cella{
	uint16_t tensione;
	uint16_t corrente;
	uint16_t temperatura;
	bool COV;// Cell x 	Overvoltage Flag
	bool CUV;// Cell x 	Undervoltage Flag
};

#endif /* INC_GLOBAL_H_ */
