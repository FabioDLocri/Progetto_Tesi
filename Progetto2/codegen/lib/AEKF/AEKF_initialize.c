/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: AEKF_initialize.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 19-Jan-2026 17:14:29
 */

/* Include Files */
#include "AEKF_initialize.h"
#include "AEKF.h"
#include "AEKF_data.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : void
 * Return Type  : void
 */
void AEKF_initialize(void)
{
  AEKF_init();
  isInitialized_AEKF = true;
}

/*
 * File trailer for AEKF_initialize.c
 *
 * [EOF]
 */
