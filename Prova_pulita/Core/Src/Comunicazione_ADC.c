/*
 * Comunicazione_ADC.c
 *
 *  Created on: Nov 14, 2025
 *      Author: faxio
 */

#include "Comunicazione_ADC.h"
#include "main.h"
#include "stm32h7xx_hal.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

extern SPI_HandleTypeDef hspi5;

#define NUM_CHANNELS 5
#define CHANNELS {0, 1, 2, 3, 4}  // AIN0, AIN1, AIN2, AIN3, AIN4


void MAX1227_Init(void) {
    // 1. Reset dell'ADC
    MAX1227_WriteByte(0x08);  // Reset register

    // 2. Configurazione Setup Register
    // Clock interno, riferimento interno, singolo-ended
    MAX1227_WriteByte(0x50);  // Setup: 01010000

    HAL_Delay(10);
}

void MAX1227_WriteByte(uint8_t data) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);  // CS low

    HAL_SPI_Transmit(&hspi5, &data, 1, 100);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);  // CS high
}


float* read_all_voltages(void) {
    float voltages[NUM_CHANNELS];
    uint8_t channels[NUM_CHANNELS] = {0, 1, 2, 3, 4};

    for(int i = 0; i < NUM_CHANNELS; i++) {
        voltages[i] = read_single_channel(channels[i]);
        HAL_Delay(1);
    }

    return voltages;
}

float read_single_channel(uint8_t channel) {
    uint16_t raw_value;
    float voltage;

    // Byte di conversione: 1 CCCX XX01
    // dove CCCX sono i bit del canale
    uint8_t conv_byte = 0x80 | (channel << 3) | 0x01;

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);  // CS low

    // Invia byte di conversione
    HAL_SPI_Transmit(&hspi5, &conv_byte, 1, 100);

    // Attendi EOC (polling o interrupt)
    while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == GPIO_PIN_SET);

    // Leggi risultato (16 bit)
    uint8_t rx_data[2] = {0};
    HAL_SPI_Receive(&hspi5, rx_data, 2, 100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);  // CS high

    // Combina i dati ricevuti
    raw_value = (rx_data[0] << 8) | rx_data[1];
    raw_value = raw_value >> 4;  // Rimuovi 4 bit MSB (zeri)

    // Calcola voltaggio (VREF = 2.5V interno)
    voltage = (raw_value * 2.5) / 4096.0;

    return voltage;
}

// Per la tensione dello shunt (probabilmente bassa tensione)
float read_shunt_voltage(void) {
    return read_single_channel(0);  // AIN0 per shunt
}

// Per le altre tensioni
float read_voltage_1(void) { return read_single_channel(1); }
float read_voltage_2(void) { return read_single_channel(2); }
float read_voltage_3(void) { return read_single_channel(3); }
float read_voltage_4(void) { return read_single_channel(4); }

