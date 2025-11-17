/*
 * global.c
 *
 *  Created on: Nov 16, 2025
 *      Author: faxio
 */


#include "global.h"
#include "main.h"
#include "stdbool.h"
uint8_t timer_flag=0;

uint8_t config[6]={0};
uint8_t configuration_data[6] = {0};

float tensione_celle[12]={0};
float tensione_GPIO[6]={0};

//Registri Status
float somma_celle=0;
float int_temperature=0;
float analog_power_supply=0;
float digital_power_supply=0;



Cella Cell_1,Cell_2,Cell_3,Cell_4,Cell_5,Cell_6,Cell_7,Cell_8,Cell_9,Cell_10,Cell_11,Cell_12;
