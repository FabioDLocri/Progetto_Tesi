/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: AEKF_types.h
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 19-Jan-2026 17:14:29
 */

#ifndef AEKF_TYPES_H
#define AEKF_TYPES_H

/* Include Files */
#include "rtwtypes.h"

/* Type Definitions */
#ifndef typedef_struct0_T
#define typedef_struct0_T
typedef struct {
  double c_cell[4];
  double r0[84];
  double r1[84];
  double c1[84];
  double soc[21];
  double ocv[21];
} struct0_T;
#endif /* typedef_struct0_T */

#endif
/*
 * File trailer for AEKF_types.h
 *
 * [EOF]
 */
