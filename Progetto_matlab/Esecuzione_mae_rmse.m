clc;
SOC_true = out.get('SOC_true');
SOC_mis = out.get('SOC_mis');
tout = out.get('tout');
Capacity_mis = out.get("Capacity_mis");

[mae, rmse] = calcolo_mae_rmse_SOC(SOC_true.Data, SOC_mis.Data,SOC_true.Time);

[mae_cap, rmse_cap] = calcolo_mae_rmse_capacity( batt.c_cell, Capacity_mis.Data, Capacity_mis.Time);