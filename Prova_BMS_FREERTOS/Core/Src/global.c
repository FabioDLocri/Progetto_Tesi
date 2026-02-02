/*
 * global.c
 *
 *  Created on: Nov 16, 2025
 *      Author: faxio
 */


#include "global.h"
#include "main.h"
#include "stdbool.h"




//Macro per accendere o spegnere i led sulla board
void ledredoff(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
};

void ledyellowoff(void){
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
};

void ledgreenoff(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
};


void ledredon(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
};

void ledyellowon(void){
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
};

void ledgreenon(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
};

void CSSPION(SPI_t spi){
	switch(spi){
	case SPI_3:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		break;
	case SPI_5 :
		break;
	}
}

void CSSPIOFF(SPI_t spi){
	switch(spi){
	case SPI_3:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		break;
	case SPI_5 :
		break;
	}
}
