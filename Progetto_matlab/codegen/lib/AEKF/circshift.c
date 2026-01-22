/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: circshift.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 19-Jan-2026 17:14:29
 */

/* Include Files */
#include "circshift.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : double a[10]
 * Return Type  : void
 */
void circshift(double a[10])
{
  double a__1;
  int k;
  a__1 = a[9];
  for (k = 8; k >= 0; k--) {
    a[k + 1] = a[k];
  }
  a[0] = a__1;
}

/*
 * File trailer for circshift.c
 *
 * [EOF]
 */
