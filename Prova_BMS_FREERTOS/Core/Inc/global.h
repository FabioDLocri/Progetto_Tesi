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




#define N_celle 12

typedef struct {
	float tensione;
	float corrente;
	float temperatura;
	bool COV;// Cell x 	Overvoltage Flag
	bool CUV;// Cell x 	Undervoltage Flag
}Cella;

typedef struct {
	Cella Cell[N_celle];
	float corrente_pacco;
	uint32_t timestamp;
}Batteria;


typedef struct {
	float somma_celle;
	float int_temperature;
	float analog_power_supply;
	float digital_power_supply;
}Status_register;

typedef struct{
	Batteria Pack;
	Status_register reg;
}Datatocom;

// Configurazione per il calcolo SoC
typedef struct {
    // Capacità cella in Ah (necessaria per il coulomb counting)
    float capacity_Ah;

    // Efficienze per correzione coulomb counting
    float eta_charge;     // es: 0.995 (perdita in carica)
    float eta_discharge;  // es: 1.000 o 0.999
} SoCConfig;

extern SoCConfig cfg;

//Macro per accendere o spegnere i led sulla board

void ledredoff(void);

void ledyellowoff(void);

void ledgreenoff(void);


void ledredon(void);

void ledyellowon(void);

void ledgreenon(void);

typedef enum{
	SPI_3,
	SPI_5
}SPI_t;

void CSSPIOFF(SPI_t spi);
void CSSPION(SPI_t spi);

#endif /* INC_GLOBAL_H_ */
