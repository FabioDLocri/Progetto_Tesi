/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: _coder_AEKF_api.h
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 19-Jan-2026 17:14:29
 */

#ifndef _CODER_AEKF_API_H
#define _CODER_AEKF_API_H

/* Include Files */
#include "emlrt.h"
#include "mex.h"
#include "tmwtypes.h"
#include <string.h>

/* Type Definitions */
#ifndef typedef_struct0_T
#define typedef_struct0_T
typedef struct {
  real_T c_cell[4];
  real_T r0[84];
  real_T r1[84];
  real_T c1[84];
  real_T soc[21];
  real_T ocv[21];
} struct0_T;
#endif /* typedef_struct0_T */

/* Variable Declarations */
extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void AEKF(real_T V_meas[4], real_T I_meas[4], boolean_T reset,
          struct0_T *batt_param, real_T SOC_out[4]);

void AEKF_api(const mxArray *const prhs[4], const mxArray **plhs);

void AEKF_atexit(void);

void AEKF_initialize(void);

void AEKF_terminate(void);

void AEKF_xil_shutdown(void);

void AEKF_xil_terminate(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for _coder_AEKF_api.h
 *
 * [EOF]
 */
