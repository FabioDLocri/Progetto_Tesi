/*
 * Stima_SOC.h
 *
 *  Created on: Nov 17, 2025
 *      Author: faxio
 */

#ifndef INC_FUNZIONI_SOC_H_
#define INC_FUNZIONI_SOC_H_



#include <stddef.h>
#include "stdlib.h"
#include "global.h"

/*


// Calcolo SoC da OCV (tensione a circuito aperto o tensione a riposo/filtrata)
// Restituisce SoC in [0..1] clampato.
float soc_from_ocv(float voltage_V);

// Aggiornamento SoC tramite coulomb counting:
// - soc: puntatore al SoC corrente (in [0..1]); verrà aggiornato in place
// - current_A: corrente in Ampere (CONVENZIONE: >0 = carica, <0 = scarica)
// - dt_s: passo temporale in secondi
// - cfg: capacità + efficienze
void soc_update_coulomb_count(float* soc, float current_A, float dt_s, const SoCConfig* cfg);

// Fusione ibrida: combina coulomb counting e OCV con fattore alpha:
// - soc_estimate: SoC attuale che verrà aggiornato (di solito proviene dal coulomb counting)
// - voltage_V: tensione cella (idealmente a riposo o ben filtrata)
// - alpha in [0..1]: 1.0 = solo CC, 0.0 = solo OCV. Tipico 0.9..0.99
void soc_update_hybrid(float* soc_estimate,
                       float voltage_V,
                       float current_A,
                       float dt_s,
                       const SoCConfig* cfg,
                       float alpha);

// Calcola il SoC per un array di celle usando il metodo OCV.
// - cell_voltages_V: array di tensioni per cella
// - out_soc: array di SoC risultante (dimensione n_cells)
// - n_cells: numero celle
void compute_cells_soc_from_voltage (Cella* cells);

void calcolo_SOC();

*/

#endif /* INC_FUNZIONI_SOC_H_ */
