/*
 * global.c
 *
 *  Created on: Nov 16, 2025
 *      Author: faxio
 */


#include "global.h"
#include "main.h"

uint8_t timer_flag=0;

uint8_t config[6]={0};

float tensione_celle[12]={0};
float tensione_GPIO[6]={0};

//Registri Status
float somma_celle=0;
float int_temperature=0;
float analog_power_supply=0;
float digital_power_supply=0;


