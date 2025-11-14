/*
 * Comunicazione_Stack.c
 *
 *  Created on: Nov 11, 2025
 *      Author: faxio
 */


#include "Comunicazione_Stack.h"
#include "main.h"
#include "stm32h7xx_hal.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

extern SPI_HandleTypeDef hspi3;

typedef struct {
    uint16_t pec : 15;
} pec_register_t;

// Calcola il PEC per un array di dati


uint16_t calculate_pec(uint8_t *data, uint8_t len) {
    uint16_t pec = 0x0010; // Valore iniziale

    for (uint8_t i = 0; i < len; i++) {
        uint8_t byte = data[i];

        for (uint8_t bit = 0; bit < 8; bit++) {
            uint8_t din = (byte >> 7) & 0x01;
            byte <<= 1;

            uint8_t in0 = din ^ ((pec >> 14) & 0x01);

            pec <<= 1;

            if (in0) pec ^= (1 << 0);
            if (in0) pec ^= (1 << 3);
            if (in0) pec ^= (1 << 4);
            if (in0) pec ^= (1 << 7);
            if (in0) pec ^= (1 << 8);
            if (in0) pec ^= (1 << 10);
            if (in0) pec ^= (1 << 14);
        }
    }

    return (pec & 0x7FFF) << 1;
}

uint8_t verify_pec(uint8_t *data, uint8_t len, uint16_t received_pec) {
    uint16_t calculated_pec = calculate_pec(data, len);
    return (calculated_pec == received_pec);
}

//Funzioni di invio dati
// Invia comando all'LTC6811
HAL_StatusTypeDef ltc6811_send_command(uint16_t command)
{
	uint8_t tx_data[4];
   // uint8_t rx_data[4];

    // Prepara comando + PEC
    tx_data[0] = (command >> 8) & 0xFF; // CMD0
    tx_data[1] = command & 0xFF;        // CMD1

    uint16_t pec = calculate_pec(tx_data, 2);
    tx_data[2] = (pec >> 8) & 0xFF;     // PEC0
    tx_data[3] = pec & 0xFF;            // PEC1

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi3, tx_data, 4, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);

    return status;
}

// Scrive dati all'LTC6811 (per comandi WRCFGA, WRPWM, etc.)
HAL_StatusTypeDef ltc6811_write_data(uint16_t command, uint8_t *data, uint8_t data_len)
{
	uint8_t tx_data[4];

// Prepara comando + PEC
    tx_data[0] = (command >> 8) & 0xFF; // CMD0
    tx_data[1] = command & 0xFF;        // CMD1

	uint16_t pec = calculate_pec(tx_data, 2);
    tx_data[2] = (pec >> 8) & 0xFF;     // PEC0
    tx_data[3] = pec & 0xFF;            // PEC1

    uint8_t tx_buffer[data_len + 2];

    // Copia dati
    for (int i = 0; i < data_len; i++) {
        tx_buffer[i] = data[i];
    }

    // Calcola e aggiungi PEC
    uint16_t pec1 = calculate_pec(data, data_len);
    tx_buffer[data_len] = (pec1 >> 8) & 0xFF;
    tx_buffer[data_len + 1] = pec1 & 0xFF;

    uint8_t tx_send[data_len + 6];
    for (int i = 0; i < 4; i++) {
        tx_send[i] = tx_data[i];
    }
    for (int i = 0; i < data_len+2; i++) {
        tx_send[4+i] = tx_buffer[i];
    }

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi3, tx_send, data_len + 6, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);

    return status;
}

// Legge dati dall'LTC6811 (per comandi RDCFGA, RDCVA, etc.)
HAL_StatusTypeDef ltc6811_read_data(uint16_t command, uint8_t *rx_data, uint8_t data_len)
{
	uint8_t tx_data[4];

	// Prepara comando + PEC
	tx_data[0] = (command >> 8) & 0xFF; // CMD0
	tx_data[1] = command & 0xFF;        // CMD1

	uint16_t pec = calculate_pec(tx_data, 2);
	tx_data[2] = (pec >> 8) & 0xFF;     // PEC0
	tx_data[3] = pec & 0xFF;            // PEC1

    uint8_t rx_buffer[data_len + 2];


    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_StatusTypeDef status1 = HAL_SPI_Transmit(&hspi3, tx_data, 4, 10);
    HAL_StatusTypeDef status = HAL_SPI_Receive(&hspi3, rx_buffer, data_len+2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);

    if (status == HAL_OK) {
        // Copia dati (escludi PEC)
        for (int i = 0; i < data_len; i++) {
            rx_data[i] = rx_buffer[i];
        }

        // Verifica PEC
        uint16_t received_pec = (rx_buffer[data_len] << 8) | rx_buffer[data_len + 1];
        if (!verify_pec(rx_data, data_len, received_pec)) {
            return HAL_ERROR;
        }
    }

    return status;
}



// Configura LTC6811
HAL_StatusTypeDef ltc6811_configure(void)
{
    uint8_t config_data[6] = {0};

    // CFGR0: GPIO pull-down OFF, REFON=0, ADCOPT=0
    config_data[0] = 0xFC; // 0b11111100 - tutti GPIO pull-down OFF

    // CFGR1: Undervoltage threshold (esempio: 3.3V)
    uint16_t vuv = 33000 / 16; // 33000mV / (16 * 0.1mV)
    config_data[1] = vuv & 0xFF;
    //config_data[2] = (vuv >> 8) & 0x0F;

    // CFGR3: Overvoltage threshold (esempio: 4.2V)
    uint16_t vov = 42000 / 16; // 42000mV / (16 * 0.1mV)
    config_data[2] = (vov << 4) | ((vuv >> 8) & 0x0F);
    config_data[3] = (vov >> 4) & 0xFF;

    // CFGR4-CFGR5: Discharge control disabilitato
    config_data[4] = 0x00;
    config_data[5] = 0x00;

    // Invia comando WRCFGA

    // Invia dati di configurazione
    return ltc6811_write_data(0x0001, config_data, 6);
}

// Legge tensioni celle
	HAL_StatusTypeDef ltc6811_read_cell_voltages(float *voltages) {
    // Avvia conversione ADC
    HAL_StatusTypeDef status = ltc6811_send_command(0x0360); // ADCV - tutte le celle, 7kHz
    if (status != HAL_OK) return status;

    // Attendi fine conversione (290μs)
    HAL_Delay(3); // 3ms per sicurezza

    // Leggi tutti i gruppi di registri
    uint8_t cell_data[48]; // 12 celle × 4 bytes (2 bytes dato + 2 bytes PEC per gruppo)

    // Leggi gruppo A (celle 1-3)
    status = ltc6811_read_data(0x0004, &cell_data[0], 6);
    if (status != HAL_OK) return status;

    // Leggi gruppo B (celle 4-6)
    status = ltc6811_read_data(0x0006, &cell_data[6], 6);
    if (status != HAL_OK) return status;

    // Leggi gruppo C (celle 7-9)
    status = ltc6811_read_data(0x0008, &cell_data[12], 6);
    if (status != HAL_OK) return status;

    // Leggi gruppo D (celle 10-12)
    status = ltc6811_read_data(0x000A, &cell_data[18], 6);
    if (status != HAL_OK) return status;

    // Decodifica tensioni
    for (int cell = 0; cell < 12; cell++) {
        int group_offset = (cell / 3) * 6;
        int cell_in_group = cell % 3;
        int byte_offset = group_offset + (cell_in_group * 2);

        uint16_t raw_voltage = (cell_data[byte_offset+1] << 8) | cell_data[byte_offset];
        voltages[cell] = raw_voltage * 0.0001f; // Converti in Volt (100μV per LSB)
    }

    return HAL_OK;
}

	// Legge tensioni GPIO
	HAL_StatusTypeDef ltc6811_read_gpio_voltages(float *gpio_voltage)
	{
	    // Avvia conversione ADC
	    HAL_StatusTypeDef status = ltc6811_send_command(0x0561); // ADAX - GPIO1, 7kHz
	    if (status != HAL_OK) return status;

	    // Attendi fine conversione
	    HAL_Delay(3); // 3ms per sicurezza

	    // Leggi tutti i gruppi di registri
	    uint8_t GPIO_data[24]; // 6 registri × 4 bytes (2 bytes dato + 2 bytes PEC per gruppo)

	    // Leggi gruppo A (GPIO 1-3)
	    status = ltc6811_read_data(0x000C, &GPIO_data[0], 6);
	    if (status != HAL_OK) return status;

	    // Leggi gruppo B (GPIO 4-5 e Second Reference)
	    status = ltc6811_read_data(0x000E, &GPIO_data[6], 6);
	    if (status != HAL_OK) return status;

	    // Decodifica tensioni
	    for (int GPIO = 0; GPIO < 6; GPIO++) {
	        int group_offset = (GPIO / 3) * 6;
	        int GPIO_in_group = GPIO % 3;
	        int byte_offset = group_offset + (GPIO_in_group * 2);

	        uint16_t raw_voltage = (GPIO_data[byte_offset+1] << 8) | GPIO_data[byte_offset];
	        gpio_voltage[GPIO] = raw_voltage * 0.0001f; // Converti in Volt (100μV per LSB)
	    }

	    return HAL_OK;
	}


		// Legge tensioni GPIO
	HAL_StatusTypeDef ltc6811_read_status_a(float *somma_celle, float *int_temperature, float *analog_power_supply)
		{
		    // Avvia conversione ADC
		    HAL_StatusTypeDef status = ltc6811_send_command(0x0568); // ADAX - GPIO1, 7kHz
		    if (status != HAL_OK) return status;

		    // Attendi fine conversione
		    HAL_Delay(3); // 3ms per sicurezza

		    // Leggi tutti i gruppi di registri
		    uint8_t Status_A[12]; // 3 registri × 4 bytes (2 bytes dato + 2 bytes PEC per gruppo)

		    // Leggi gruppo A (SC, ITMP, VA)
		    status = ltc6811_read_data(0x0010, &Status_A[0], 6);
		    if (status != HAL_OK) return status;

		    // Decodifica tensioni

		        *somma_celle= ((Status_A[1] << 8) | Status_A[0]) * 0.0001f*20; // Converti in Volt (100μV per LSB)

		        *int_temperature=((((Status_A[3] << 8) | Status_A[2]) * 0.0001f)/0.0075f)-273; //Converti in temperatura

		        *analog_power_supply= ((Status_A[5] << 8) | Status_A[4]) * 0.0001f; // Converti in Volt (100μV per LSB)

		    return HAL_OK;
		}
