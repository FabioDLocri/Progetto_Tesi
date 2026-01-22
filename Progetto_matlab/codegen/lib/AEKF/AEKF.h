/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: AEKF.h
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 19-Jan-2026 17:14:29
 */

#ifndef AEKF_H
#define AEKF_H

/* Include Files */
#include "AEKF_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
extern void AEKF(const double V_meas[4], const double I_meas[4],
                 boolean_T reset, const struct0_T *batt_param,
                 double SOC_out[4]);

void AEKF_init(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for AEKF.h
 *
 * [EOF]
 */
