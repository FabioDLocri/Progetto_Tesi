/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: main.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 19-Jan-2026 17:14:29
 */

/*************************************************************************/
/* This automatically generated example C main file shows how to call    */
/* entry-point functions that MATLAB Coder generated. You must customize */
/* this file for your application. Do not modify this file directly.     */
/* Instead, make a copy of this file, modify it, and integrate it into   */
/* your development environment.                                         */
/*                                                                       */
/* This file initializes entry-point function arguments to a default     */
/* size and value before calling the entry-point functions. It does      */
/* not store or use any values returned from the entry-point functions.  */
/* If necessary, it does pre-allocate memory for returned values.        */
/* You can use this file as a starting point for a main function that    */
/* you can deploy in your application.                                   */
/*                                                                       */
/* After you copy the file, and before you deploy it, you must make the  */
/* following changes:                                                    */
/* * For variable-size function arguments, change the example sizes to   */
/* the sizes that your application requires.                             */
/* * Change the example values of function arguments to the values that  */
/* your application requires.                                            */
/* * If the entry-point functions return values, store these values or   */
/* otherwise use them as required by your application.                   */
/*                                                                       */
/*************************************************************************/

/* Include Files */
#include "main.h"
#include "AEKF.h"
#include "AEKF_initialize.h"
#include "AEKF_terminate.h"
#include "AEKF_types.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Declarations */
static void argInit_1x21_real_T(double result[21]);

static void argInit_4x1_real_T(double result[4]);

static void argInit_4x21_real_T(double result[84]);

static boolean_T argInit_boolean_T(void);

static double argInit_real_T(void);

static void argInit_struct0_T(struct0_T *result);

/* Function Definitions */
/*
 * Arguments    : double result[21]
 * Return Type  : void
 */
static void argInit_1x21_real_T(double result[21])
{
  int idx1;
  /* Loop over the array to initialize each element. */
  for (idx1 = 0; idx1 < 21; idx1++) {
    /* Set the value of the array element.
Change this value to the value that the application requires. */
    result[idx1] = argInit_real_T();
  }
}

/*
 * Arguments    : double result[4]
 * Return Type  : void
 */
static void argInit_4x1_real_T(double result[4])
{
  int idx0;
  /* Loop over the array to initialize each element. */
  for (idx0 = 0; idx0 < 4; idx0++) {
    /* Set the value of the array element.
Change this value to the value that the application requires. */
    result[idx0] = argInit_real_T();
  }
}

/*
 * Arguments    : double result[84]
 * Return Type  : void
 */
static void argInit_4x21_real_T(double result[84])
{
  int i;
  /* Loop over the array to initialize each element. */
  for (i = 0; i < 84; i++) {
    /* Set the value of the array element.
Change this value to the value that the application requires. */
    result[i] = argInit_real_T();
  }
}

/*
 * Arguments    : void
 * Return Type  : boolean_T
 */
static boolean_T argInit_boolean_T(void)
{
  return false;
}

/*
 * Arguments    : void
 * Return Type  : double
 */
static double argInit_real_T(void)
{
  return 0.0;
}

/*
 * Arguments    : struct0_T *result
 * Return Type  : void
 */
static void argInit_struct0_T(struct0_T *result)
{
  int i;
  /* Set the value of each structure field.
Change this value to the value that the application requires. */
  argInit_4x21_real_T(result->r0);
  argInit_1x21_real_T(result->soc);
  argInit_4x1_real_T(result->c_cell);
  for (i = 0; i < 84; i++) {
    result->r1[i] = result->r0[i];
    result->c1[i] = result->r0[i];
  }
  memcpy(&result->ocv[0], &result->soc[0], 21U * sizeof(double));
}

/*
 * Arguments    : int argc
 *                char **argv
 * Return Type  : int
 */
int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  /* Initialize the application.
You do not need to do this more than one time. */
  AEKF_initialize();
  /* Invoke the entry-point functions.
You can call entry-point functions multiple times. */
  main_AEKF();
  /* Terminate the application.
You do not need to do this more than one time. */
  AEKF_terminate();
  return 0;
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void main_AEKF(void)
{
  struct0_T r;
  double SOC_out[4];
  double V_meas_tmp[4];
  /* Initialize function 'AEKF' input arguments. */
  /* Initialize function input argument 'V_meas'. */
  argInit_4x1_real_T(V_meas_tmp);
  /* Initialize function input argument 'I_meas'. */
  /* Initialize function input argument 'batt_param'. */
  /* Call the entry-point 'AEKF'. */
  argInit_struct0_T(&r);
  AEKF(V_meas_tmp, V_meas_tmp, argInit_boolean_T(), &r, SOC_out);
}

/*
 * File trailer for main.c
 *
 * [EOF]
 */
