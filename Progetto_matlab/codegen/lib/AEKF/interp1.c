/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: interp1.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 19-Jan-2026 17:14:29
 */

/* Include Files */
#include "interp1.h"
#include "rt_nonfinite.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : const double varargin_1[21]
 *                const double varargin_2[21]
 *                double varargin_3
 * Return Type  : double
 */
double interp1(const double varargin_1[21], const double varargin_2[21],
               double varargin_3)
{
  double x[21];
  double y[21];
  double Vq;
  int b_j1;
  memcpy(&y[0], &varargin_2[0], 21U * sizeof(double));
  memcpy(&x[0], &varargin_1[0], 21U * sizeof(double));
  if (varargin_1[1] < varargin_1[0]) {
    for (b_j1 = 0; b_j1 < 10; b_j1++) {
      Vq = x[b_j1];
      x[b_j1] = x[20 - b_j1];
      x[20 - b_j1] = Vq;
      Vq = y[b_j1];
      y[b_j1] = y[20 - b_j1];
      y[20 - b_j1] = Vq;
    }
  }
  if (rtIsNaN(varargin_3)) {
    Vq = rtNaN;
  } else if (varargin_3 > x[20]) {
    Vq = y[20] + (varargin_3 - x[20]) / (x[20] - x[19]) * (y[20] - y[19]);
  } else if (varargin_3 < x[0]) {
    Vq = y[0] + (varargin_3 - x[0]) / (x[1] - x[0]) * (y[1] - y[0]);
  } else {
    double r;
    int high_i;
    int low_i;
    int low_ip1;
    low_i = 1;
    low_ip1 = 2;
    high_i = 21;
    while (high_i > low_ip1) {
      int mid_i;
      mid_i = (low_i + high_i) >> 1;
      if (varargin_3 >= x[mid_i - 1]) {
        low_i = mid_i;
        low_ip1 = mid_i + 1;
      } else {
        high_i = mid_i;
      }
    }
    Vq = x[low_i - 1];
    r = (varargin_3 - Vq) / (x[low_i] - Vq);
    if (r == 0.0) {
      Vq = y[low_i - 1];
    } else if (r == 1.0) {
      Vq = y[low_i];
    } else {
      Vq = y[low_i - 1];
      if (!(Vq == y[low_i])) {
        Vq = (1.0 - r) * Vq + r * y[low_i];
      }
    }
  }
  return Vq;
}

/*
 * File trailer for interp1.c
 *
 * [EOF]
 */
