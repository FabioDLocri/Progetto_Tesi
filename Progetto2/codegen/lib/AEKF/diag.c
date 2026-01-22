/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: diag.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 19-Jan-2026 17:14:29
 */

/* Include Files */
#include "diag.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const double v[2]
 *                double d[4]
 * Return Type  : void
 */
void diag(const double v[2], double d[4])
{
  d[1] = 0.0;
  d[2] = 0.0;
  d[0] = v[0];
  d[3] = v[1];
}

/*
 * File trailer for diag.c
 *
 * [EOF]
 */
