/*
 * Stima_SOC.c
 *
 *  Created on: Nov 17, 2025
 *      Author: faxio
 */


#include "main.h"
#include "funzioni_SOC.h"
#include "stm32h7xx_hal.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "global.h"
#include "Config_LTC6811.h"
#include "stdbool.h"

//tabelle corrispondenti alla curva OCV_SOC

float ocv_table[] = {
		3270.7, 3320.1, 3362.5, 3378.6, 3394.7, 3410.9, 3427.0, 3443.1, 3459.2, 3475.3,
		    3491.4, 3507.5, 3521.4, 3533.1, 3544.8, 3556.6, 3568.3, 3580.0, 3591.7, 3603.4,
		    3615.2, 3626.9, 3637.6, 3647.9, 3658.3, 3668.7, 3679.0, 3689.4, 3699.7, 3710.1,
		    3720.5, 3730.4, 3734.5, 3738.6, 3742.7, 3746.8, 3750.9, 3755.0, 3759.1, 3763.2,
		    3767.3, 3770.5, 3772.0, 3773.4, 3774.9, 3776.4, 3777.8, 3779.3, 3780.8, 3782.3,
		    3783.7, 3787.2, 3791.9, 3796.6, 3801.3, 3806.0, 3810.7, 3815.4, 3820.1, 3824.8,
		    3829.5, 3834.4, 3839.2, 3844.0, 3848.9, 3853.7, 3858.6, 3863.4, 3868.3, 3873.1,
		    3880.2, 3889.3, 3898.5, 3907.6, 3916.8, 3926.0, 3935.1, 3944.3, 3953.5, 3962.6,
		    3972.3, 3982.1, 3991.8, 4001.5, 4011.2, 4021.0, 4030.7, 4040.4, 4050.1, 4060.2,
		    4070.8, 4081.3, 4091.9, 4102.5, 4113.0, 4123.6, 4134.2, 4144.7, 4155.3, 4164.9, 4174.3
};

float soc_table[] = {
    0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0,
    10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0,
    20.0, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0,
    30.0, 31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0,
    40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0, 48.0, 49.0,
    50.0, 51.0, 52.0, 53.0, 54.0, 55.0, 56.0, 57.0, 58.0, 59.0,
    60.0, 61.0, 62.0, 63.0, 64.0, 65.0, 66.0, 67.0, 68.0, 69.0,
    70.0, 71.0, 72.0, 73.0, 74.0, 75.0, 76.0, 77.0, 78.0, 79.0,
    80.0, 81.0, 82.0, 83.0, 84.0, 85.0, 86.0, 87.0, 88.0, 89.0,
    90.0, 91.0, 92.0, 93.0, 94.0, 95.0, 96.0, 97.0, 98.0, 99.0, 100.0
};


float clamp01(float x) {
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// Interpolazione lineare inversa OCV->SoC:
// Assunzione: cfg->ocv_v è crescente, cfg->soc è monotono e in [0..1].
float soc_from_ocv(float voltage_V) {

    // Clamp se fuori dalla tabella
    if (voltage_V <= ocv_table[0]) return clamp01(soc_table[0]);
    if (voltage_V >= ocv_table[100]) return clamp01(soc_table[100]);

    // Trova segmento [i, i+1] tale che ocv_v[i] <= V < ocv_v[i+1]
    size_t lo = 0, hi = 100;
    while (hi - lo > 1) {
        size_t mid = lo + (hi - lo) / 2;
        if (ocv_table[mid] <= voltage_V) lo = mid;
        else hi = mid;
    }

    float v0 = ocv_table[lo];
    float v1 = ocv_table[lo + 1];
    float s0 = soc_table[lo];
    float s1 = soc_table[lo + 1];

    float t = (voltage_V - v0) / (v1 - v0);
    float s = lerp(s0, s1, t);
    return clamp01(s);
}

// Coulomb counting: dSoC = (I * dt / 3600) / Capacity, con efficienze.
// Convenzione: I > 0 carica => SoC aumenta; I < 0 scarica => SoC diminuisce.
void soc_update_coulomb_count(float* soc, float current_A, float dt_s, const SoCConfig* cfg) {
    if (!soc || cfg->capacity_Ah <= 0.0f || dt_s <= 0.0f) return;

    float eta = 1.0f;
    if (current_A > 0.0f) {
        eta = (cfg->eta_charge > 0.0f) ? cfg->eta_charge : 1.0f;
    } else if (current_A < 0.0f) {
        eta = (cfg->eta_discharge > 0.0f) ? cfg->eta_discharge : 1.0f;
    }

    // Ah trasferiti in questo dt
    float dAh = (current_A * dt_s / 3600.0f) * eta;

    // dSoC = dQ / Qn
    float dsoc = dAh / cfg->capacity_Ah;

    float s = *soc + dsoc;
    *soc = clamp01(s);
}

// Fusione ibrida semplice: prima aggiorna CC, poi sposta verso OCV con fattore (1-alpha).
void soc_update_hybrid(float* soc_estimate,
                       float voltage_V,
                       float current_A,
                       float dt_s,
                       const SoCConfig* cfg,
                       float alpha) {
    if (!soc_estimate) return;
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;

    // Step 1: integrazione coulomb counting
    soc_update_coulomb_count(soc_estimate, current_A, dt_s, cfg);

    // Step 2: correzione con OCV
    float soc_ocv = soc_from_ocv(voltage_V);
    float soc_cc = *soc_estimate;

    float soc_fused = alpha * soc_cc + (1.0f - alpha) * soc_ocv;
    *soc_estimate = clamp01(soc_fused);
}

void compute_cells_soc_from_voltage(Cella* cells)
{
    if (!cells) return;

    for (size_t i = 0; i < NUM_CELLS; ++i) {
        cells[i].SOC = soc_from_ocv(cells[i].tensione);
    }
}
