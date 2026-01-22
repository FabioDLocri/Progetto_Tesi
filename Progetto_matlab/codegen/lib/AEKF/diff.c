/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: diff.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 19-Jan-2026 17:14:29
 */

/* Include Files */
#include "diff.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const double x[21]
 *                double y[20]
 * Return Type  : void
 */
void diff(const double x[21], double y[20])
{
  double work;
  int m;
  work = x[0];
  for (m = 0; m < 20; m++) {
    double tmp2;
    tmp2 = work;
    work = x[m + 1];
    y[m] = work - tmp2;
  }
}

/*
 * File trailer for diff.c
 *
 * [EOF]
 */
