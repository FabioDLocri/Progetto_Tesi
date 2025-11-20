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
#include "global.h"
#include "Config_LTC6811.h"
#include "stdbool.h"
#include "cmsis_os2.h"
#include "Tasks.h"


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

//Creo il comando per ADC voltage
uint16_t LTC6811_adcv(
  uint8_t MD, //ADC Mode
  uint8_t DCP, //Discharge Permit
  uint8_t CH //Cell Channels to be measured
  )
{
	uint16_t cmd = (1<<9) + (MD<<7) + 0x60 + (DCP<<4) + CH;
	return cmd;
}

//Creo il comando per ADC voltage
uint16_t LTC6811_adax(
  uint8_t MD, //ADC Mode
  uint8_t CHG //GPIO Channels to be measured
  )
{
	uint16_t cmd = (1<<10) + (MD<<7) + 0x60 + CHG;
	return cmd;
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
    HAL_StatusTypeDef status = HAL_SPI_Transmit_IT(&hspi3, tx_data, 4);
    osSemaphoreAcquire(SPITXSemHandle, osWaitForever);
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
    HAL_StatusTypeDef status = HAL_SPI_Transmit_IT(&hspi3, tx_send, data_len + 6);
    osSemaphoreAcquire(SPITXSemHandle, osWaitForever);
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
    HAL_StatusTypeDef status1 = HAL_SPI_Transmit_IT(&hspi3, tx_data, 4);
    osSemaphoreAcquire(SPITXSemHandle, osWaitForever);
    HAL_StatusTypeDef status = HAL_SPI_Receive_IT(&hspi3, rx_buffer, data_len+2);
    osSemaphoreAcquire(SPIRXSemHandle, osWaitForever);
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

uint8_t* ltc6811_set_config(
		bool gpio[5], //GPIO pull-down ON o OFF
		bool refon,
		bool dten,
		bool adcopt,
		uint16_t uv,
		uint16_t ov,
		bool dcc[12],
		bool dcto[4]
		)
{

    configuration_data[0]= (refon<<2)+(dten<<1)+adcopt;
    // CFGR0: GPIO pull-down OFF, REFON=0, ADCOPT=0
    for(int i=0; i<5; i++)
    {
    	configuration_data[0] += (gpio[i]<<(i+3));
    };
    //0xFC; // 0b11111100 - tutti GPIO pull-down OFF

    // CFGR1: Undervoltage threshold (esempio: 3.3V)
    uint16_t vuv = uv / 16; // 3300mV / (16 * 0.1mV)
    configuration_data[1] = vuv & 0xFF;

    // CFGR3: Overvoltage threshold (esempio: 4.2V)
    uint16_t vov = ov / 16; // 4200mV / (16 * 0.1mV)
    configuration_data[2] = (vov << 4) | ((vuv >> 8) & 0x0F);
    configuration_data[3] = (vov >> 4) & 0xFF;

    // CFGR4-CFGR5: Discharge control disabilitato
    for (int i=0; i<8; i++)
        {
    	configuration_data[4] += dcc[i]<<i;
        };
    for (int i=0; i<4; i++)
        {
    	configuration_data[5] += (dcto[i]<<(i+4));
    	configuration_data[5] += dcc[i+8]<<i;
        };

    return configuration_data;
}



// Configura LTC6811
HAL_StatusTypeDef ltc6811_configure(void)
{

    // Invia comando WRCFGA
	bool gpio_default[5] = {1,1,1,1,1};
	bool dcc_default[12] = {0};
	bool dcto_default[4] = {0};

	uint8_t* config_data=ltc6811_set_config(gpio_default, 0, 0, 0, UV_THRESHOLD, OV_THRESHOLD, dcc_default, dcto_default);
    // Invia dati di configurazione
    return ltc6811_write_data(0x0001, config_data, 6);
}

// Legge tensioni celle
	HAL_StatusTypeDef ltc6811_read_cell_voltages() {
    // Avvia conversione ADC creando prima il comando a seconda delle impostazioni volute
	uint16_t cmd= LTC6811_adcv(MD_7KHZ_3KHZ,DCP_DISABLED, CELL_CH_ALL);
    HAL_StatusTypeDef status = ltc6811_send_command(cmd); // ADCV - tutte le celle, 7kHz
    if (status != HAL_OK) return status;

    // Attendi fine conversione (290μs)
    osDelay(3); // 3ms per sicurezza

    // Leggi tutti i gruppi di registri
    uint8_t cell_data[24]; // 12 celle × 2 bytes

    // Leggi gruppo A (celle 1-3)
    status = ltc6811_read_data(RDCVA, &cell_data[0], 6);
    if (status != HAL_OK) return status;

    // Leggi gruppo B (celle 4-6)
    status = ltc6811_read_data(RDCVB, &cell_data[6], 6);
    if (status != HAL_OK) return status;

    // Leggi gruppo C (celle 7-9)
    status = ltc6811_read_data(RDCVC, &cell_data[12], 6);
    if (status != HAL_OK) return status;

    // Leggi gruppo D (celle 10-12)
    status = ltc6811_read_data(RDCVD, &cell_data[18], 6);
    if (status != HAL_OK) return status;

    // Decodifica tensioni
    for (int cell = 0; cell < 12; cell++) {
        int group_offset = (cell / 3) * 6;
        int cell_in_group = cell % 3;
        int byte_offset = group_offset + (cell_in_group * 2);

        uint16_t raw_voltage = (cell_data[byte_offset+1] << 8) | cell_data[byte_offset];
        Batteria[cell].tensione = raw_voltage * 0.0001f; // Converti in Volt (100μV per LSB)
    }

    return HAL_OK;
}

	// Legge tensioni GPIO
	HAL_StatusTypeDef ltc6811_read_gpio_voltages(float *gpio_voltage)
	{
		// Avvia conversione ADC creando prima il comando a seconda delle impostazioni volute
		uint16_t cmd= LTC6811_adax(MD_7KHZ_3KHZ, AUX_CH_ALL);
	    HAL_StatusTypeDef status = ltc6811_send_command(cmd); // ADAX - GPIO1, 7kHz
	    if (status != HAL_OK) return status;

	    // Attendi fine conversione
	    osDelay(3); // 3ms per sicurezza

	    // Leggi tutti i gruppi di registri
	    uint8_t GPIO_data[12]; // 6 registri × 2 bytes

	    // Leggi gruppo A (GPIO 1-3)
	    status = ltc6811_read_data(RDAUXA, &GPIO_data[0], 6);
	    if (status != HAL_OK) return status;

	    // Leggi gruppo B (GPIO 4-5 e Second Reference)
	    status = ltc6811_read_data(RDAUXB, &GPIO_data[6], 6);
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
	HAL_StatusTypeDef ltc6811_read_status()
		{
		    // Avvia conversione ADC
		    HAL_StatusTypeDef status = ltc6811_send_command(0x0568); //ADSTAT 7kHz
		    if (status != HAL_OK) return status;

		    // Attendi fine conversione
		    osDelay(3); // 3ms per sicurezza

		    // Leggi tutti i gruppi di registri
		    uint8_t Status_A[6]; // 3 registri × 2 bytes

		    uint8_t Status_B[6]; // 3 registri × 2 bytes

		    // Leggi gruppo A (SC, ITMP, VA)
		    status = ltc6811_read_data(RDSTATA, &Status_A[0], 6);
		    if (status != HAL_OK) return status;

		    // Leggi gruppo B (VD, CxOV, CxUV)
		    status = ltc6811_read_data(RDSTATB, &Status_B[0], 6);
		    if (status != HAL_OK) return status;

		    // Decodifica tensioni

	        somma_celle= ((Status_A[1] << 8) | Status_A[0]) * 0.0001f*20; // Converti in Volt (100μV per LSB)

	        int_temperature=((((Status_A[3] << 8) | Status_A[2]) * 0.0001f)/0.0075f)-273; //Converti in temperatura

	        analog_power_supply= ((Status_A[5] << 8) | Status_A[4]) * 0.0001f; // Converti in Volt (100μV per LSB)

	        digital_power_supply= ((Status_B[1] << 8) | Status_B[0]) * 0.0001f; // Converti in Volt (100μV per LSB)

	        for(int i=0;i<4;i++)
	        {
	        	Batteria[i].CUV=(Status_B[2]>>(2*i))&(0x01);
	        	Batteria[i+4].CUV=(Status_B[3]>>(2*i))&(0x01);
	        	Batteria[i+8].CUV=(Status_B[4]>>(2*i))&(0x01);

	        	Batteria[i].COV=(Status_B[2]>>(2*i+1))&(0x01);
	        	Batteria[i+4].COV=(Status_B[3]>>(2*i+1))&(0x01);
	        	Batteria[i+8].COV=(Status_B[4]>>(2*i+1))&(0x01);
	        }

		    return HAL_OK;
		}
