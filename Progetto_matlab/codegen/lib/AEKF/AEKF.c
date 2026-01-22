/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: AEKF.c
 *
 * MATLAB Coder version            : 25.2
 * C/C++ source code generated on  : 19-Jan-2026 17:14:29
 */

/* Include Files */
#include "AEKF.h"
#include "AEKF_data.h"
#include "AEKF_initialize.h"
#include "AEKF_types.h"
#include "circshift.h"
#include "diag.h"
#include "diff.h"
#include "interp1.h"
#include "rt_nonfinite.h"
#include <emmintrin.h>
#include <math.h>
#include <string.h>

/* Variable Definitions */
static boolean_T initialized_not_empty;

/* Function Definitions */
/*
 * Ingressi:
 *    V_meas - Vettore tensioni misurate [n_cells x 1]
 *    I_meas - Vettore correnti misurate [n_cells x 1]
 *    reset  - Segnale di reset (boolean)
 *
 *  Uscita:
 *    SOC_out - SOC stimato [n_cells x 1]
 *
 * Arguments    : const double V_meas[4]
 *                const double I_meas[4]
 *                boolean_T reset
 *                const struct0_T *batt_param
 *                double SOC_out[4]
 * Return Type  : void
 */
void AEKF(const double V_meas[4], const double I_meas[4], boolean_T reset,
          const struct0_T *batt_param, double SOC_out[4])
{
  static const double dv1[2] = {0.05, 0.01};
  static double P_state[16];
  static double H_k_vector[10];
  static double Q_cov[4];
  static double SOC_state[4];
  static double V1_state[4];
  static double R_var;
  __m128d r;
  double dOCV_tab[21];
  double dOCV_temp[20];
  double A[4];
  double P_pred[4];
  double b_A[4];
  double dv[2];
  double x[2];
  double dSOC;
  int b_k;
  int k;
  if (!isInitialized_AEKF) {
    AEKF_initialize();
  }
  /* finestra di misura per AEKF */
  /*  Numero di celle  */
  /*  Parametri batteria */
  /*  Tempo campionamento [s] */
  /*  Capacità [Ah] */
  /*  Resistenza ohmica [Ohm] */
  /*  Resistenza RC [Ohm] */
  /*  Capacità RC [F] */
  /*  Pre-calcola derivata OCV */
  dSOC = batt_param->soc[1] - batt_param->soc[0];
  diff(batt_param->ocv, dOCV_temp);
  for (k = 0; k <= 18; k += 2) {
    r = _mm_loadu_pd(&dOCV_temp[k]);
    _mm_storeu_pd(&dOCV_temp[k], _mm_div_pd(r, _mm_set1_pd(dSOC)));
  }
  memset(&dOCV_tab[0], 0, 21U * sizeof(double));
  memcpy(&dOCV_tab[0], &dOCV_temp[0], 20U * sizeof(double));
  dOCV_tab[20] = dOCV_temp[19];
  /*  % Rumore EKF */
  /*  Q_cov = diag([1e-5, 1e-3]); */
  /*  R_var = 0.00001; */
  if ((!initialized_not_empty) || reset) {
    SOC_state[0] = 0.5;
    V1_state[0] = 0.0;
    SOC_state[1] = 0.5;
    V1_state[1] = 0.0;
    SOC_state[2] = 0.5;
    V1_state[2] = 0.0;
    SOC_state[3] = 0.5;
    V1_state[3] = 0.0;
    memset(&P_state[0], 0, 16U * sizeof(double));
    memset(&H_k_vector[0], 0, 10U * sizeof(double));
    dv[0] = 0.0;
    dv[1] = 1.0;
    r = _mm_loadu_pd(&dv[0]);
    _mm_storeu_pd(&x[0], _mm_add_pd(_mm_set1_pd(1.0E-5),
                                    _mm_mul_pd(_mm_set1_pd(0.00099), r)));
    diag(x, Q_cov);
    R_var = 1.0E-5;
    diag(dv1, &P_state[0]);
    diag(dv1, &P_state[4]);
    diag(dv1, &P_state[8]);
    diag(dv1, &P_state[12]);
    initialized_not_empty = true;
  }
  /*  PRE-ALLOCAZIONE */
  /*  EKF PER OGNI CELLA */
  for (b_k = 0; b_k < 4; b_k++) {
    __m128d r1;
    double b_batt_param[21];
    double K_idx_0;
    double Q_cov_tmp;
    double SOC_pred;
    double alpha;
    double d;
    double d1;
    double x_idx_0;
    double y_tilde;
    int i;
    /*  Stato corrente */
    /*     %% PREDIZIONE  */
    /*  Coulomb counting */
    Q_cov_tmp = I_meas[b_k];
    SOC_pred = fmax(0.0, fmin(1.0, SOC_state[b_k] -
                                       0.1 * Q_cov_tmp /
                                           (batt_param->c_cell[b_k] * 3600.0)));
    /* limito il soc tra 0 e 1 */
    /*  ESTRAGGO LA RIGA k DALLA MATRICE E FACCIO L'INTERPOLAZIONE */
    /*  Riga k, vettore [1 x 21] */
    /*  Converte a colonna per interp1 */
    /*  [21 x 1] */
    /*  Interpolazione */
    for (k = 0; k < 21; k++) {
      b_batt_param[k] = batt_param->r1[b_k + (k << 2)];
    }
    dSOC = interp1(batt_param->soc, b_batt_param, SOC_pred);
    for (k = 0; k < 21; k++) {
      b_batt_param[k] = batt_param->c1[b_k + (k << 2)];
    }
    __m128d r2;
    __m128d r3;
    alpha =
        exp(-0.1 / (dSOC * interp1(batt_param->soc, b_batt_param, SOC_pred)));
    /*  Dinamica RC */
    d = alpha * V1_state[b_k] + dSOC * (1.0 - alpha) * Q_cov_tmp;
    /*  Jacobiano stato */
    A[0] = 1.0;
    A[2] = 0.0;
    A[1] = 0.0;
    A[3] = alpha;
    memset(&b_A[0], 0, sizeof(double) << 2);
    r = _mm_loadu_pd(&A[0]);
    r1 = _mm_loadu_pd(&b_A[0]);
    i = b_k << 2;
    _mm_storeu_pd(&b_A[0],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(P_state[i]))));
    r = _mm_loadu_pd(&A[2]);
    r1 = _mm_loadu_pd(&b_A[0]);
    _mm_storeu_pd(&b_A[0],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(P_state[i + 1]))));
    r = _mm_loadu_pd(&A[0]);
    r1 = _mm_loadu_pd(&b_A[2]);
    _mm_storeu_pd(&b_A[2],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(P_state[i + 2]))));
    r = _mm_loadu_pd(&A[2]);
    r1 = _mm_loadu_pd(&b_A[2]);
    _mm_storeu_pd(&b_A[2],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(P_state[i + 3]))));
    r2 = _mm_loadu_pd(&b_A[2]);
    r = _mm_mul_pd(r2, _mm_set1_pd(0.0));
    r3 = _mm_loadu_pd(&b_A[0]);
    r = _mm_add_pd(r3, r);
    r1 = _mm_loadu_pd(&Q_cov[0]);
    r = _mm_add_pd(r, r1);
    _mm_storeu_pd(&P_pred[0], r);
    r1 = _mm_mul_pd(r3, _mm_set1_pd(0.0));
    r = _mm_mul_pd(r2, _mm_set1_pd(alpha));
    r = _mm_add_pd(r1, r);
    r1 = _mm_loadu_pd(&Q_cov[2]);
    r = _mm_add_pd(r, r1);
    _mm_storeu_pd(&P_pred[2], r);
    /*     %% AGGIORNAMENTO */
    for (k = 0; k < 21; k++) {
      b_batt_param[k] = batt_param->r0[b_k + (k << 2)];
    }
    y_tilde = V_meas[b_k] -
              ((interp1(batt_param->soc, batt_param->ocv, SOC_pred) - d) -
               interp1(batt_param->soc, b_batt_param, SOC_pred) * Q_cov_tmp);
    x_idx_0 = interp1(batt_param->soc, dOCV_tab, SOC_pred);
    memset(&x[0], 0, sizeof(double) << 1);
    dSOC = x_idx_0 * P_pred[0];
    alpha = (((x[0] + dSOC) - P_pred[1]) * x_idx_0 -
             ((x[1] + x_idx_0 * P_pred[2]) - P_pred[3])) +
            R_var;
    /*  Aggiornamento stato */
    d1 = (dSOC - P_pred[2]) / alpha;
    K_idx_0 = d1;
    dSOC = SOC_pred + d1 * y_tilde;
    d1 = (P_pred[1] * x_idx_0 - P_pred[3]) / alpha;
    SOC_pred = fmax(0.0, fmin(1.0, dSOC));
    /*  Aggiornamento covarianza */
    b_A[0] = 1.0 - K_idx_0 * x_idx_0;
    b_A[1] = 0.0 - d1 * x_idx_0;
    b_A[2] = 0.0 - (-K_idx_0);
    b_A[3] = 1.0 - (-d1);
    memset(&A[0], 0, sizeof(double) << 2);
    r = _mm_loadu_pd(&b_A[0]);
    r1 = _mm_loadu_pd(&A[0]);
    _mm_storeu_pd(&A[0], _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(P_pred[0]))));
    r = _mm_loadu_pd(&b_A[2]);
    r1 = _mm_loadu_pd(&A[0]);
    _mm_storeu_pd(&A[0], _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(P_pred[1]))));
    r = _mm_loadu_pd(&b_A[0]);
    r1 = _mm_loadu_pd(&A[2]);
    _mm_storeu_pd(&A[2], _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(P_pred[2]))));
    r = _mm_loadu_pd(&b_A[2]);
    r1 = _mm_loadu_pd(&A[2]);
    _mm_storeu_pd(&A[2], _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(P_pred[3]))));
    circshift(H_k_vector);
    dSOC = y_tilde * y_tilde;
    H_k_vector[0] = dSOC;
    if (H_k_vector[9] != 0.0) {
      for (k = 0; k < 9; k++) {
        dSOC += H_k_vector[k + 1];
      }
      dSOC /= 10.0;
      memset(&x[0], 0, sizeof(double) << 1);
      Q_cov_tmp = K_idx_0 * dSOC;
      Q_cov[0] = Q_cov_tmp * K_idx_0;
      alpha = d1 * dSOC;
      Q_cov[1] = alpha * K_idx_0;
      Q_cov[2] = Q_cov_tmp * d1;
      Q_cov[3] = alpha * d1;
      R_var = dSOC - (((x[0] + x_idx_0 * A[0]) - A[1]) * x_idx_0 -
                      ((x[1] + x_idx_0 * A[2]) - A[3]));
    }
    /*     %% SALVATAGGIO  */
    SOC_state[b_k] = SOC_pred;
    V1_state[b_k] = d + d1 * y_tilde;
    P_state[i] = A[0];
    P_state[i + 1] = A[1];
    P_state[i + 2] = A[2];
    P_state[i + 3] = A[3];
    SOC_out[b_k] = SOC_pred;
  }
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void AEKF_init(void)
{
  initialized_not_empty = false;
}

/*
 * File trailer for AEKF.c
 *
 * [EOF]
 */
