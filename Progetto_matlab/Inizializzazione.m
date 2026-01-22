%% Script di Inizializzazione per la Simulazione del BMS
clear; clc;


fprintf('Esecuzione script di inizializzazione...\n');

load("data/input/NMC_60Ah_param.mat", "mdl_par");



cell.nominal_voltage = 3.65; %V
cell.capacity = 60;   %Ah, della batteria originale caratterizzata
cell.ocv = mdl_par.ocv;
cell.soc = mdl_par.soc/100;
cell.r0 = mdl_par.r0;
cell.r1 = mdl_par.r1;
cell.c1 = mdl_par.c1;


%%%%%%%%%%%%%%% parametri delle celle %%%%%%%%%%%%%%%%%%%%
Ncell=4;         %
c_nom=1;        % [Ah] il fattore 1/3600 per convertire da [As] lo considero quando integro la corrente per calcolare il SoC in simulink per
c_var=0.1;
batt.soc0 = 1 + zeros(Ncell,1);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


c_min=c_nom*(1-c_var);
c_max=c_nom*(1+c_var);

batt.ncell = Ncell;
batt.c_nom = c_nom;
batt.c_var = 0.05;
batt.c_cell =  (c_max:-(c_max-c_min)/(Ncell-1):c_min)';
batt.r0 = (cell.r0*cell.capacity) ./ batt.c_cell;       %scalo i risultati per la capacità che considero
batt.r1= (cell.r1*cell.capacity) ./ batt.c_cell;
batt.c1= (cell.c1*cell.capacity) ./ batt.c_cell;
batt.soc=cell.soc;
batt.ocv=cell.ocv;
batt.soc0 = 1 + zeros(Ncell,1);

%uso gli stessi parametri precedenti, ma facendo in modo che l'AEKF non
%sappia la Capacità vera delle celle.
pack.ncell = Ncell;
pack.c_nom = batt.c_cell; %c_nom;
pack.c_cell=([1 1 1 1].*c_nom)';
pack.r0=cell.r0*cell.capacity ./ pack.c_cell;
pack.r1=cell.r1*cell.capacity ./ pack.c_cell;
pack.c1=cell.c1*cell.capacity ./ pack.c_cell;
pack.soc=cell.soc;
pack.ocv=cell.ocv;
pack.soc0 = 1 + zeros(Ncell,1);

% Parametri Bilanciamento DC-DC

pack.I_bal_max = 1;      % Corrente max bilanciamento [A]
pack.eta_dcdc = 0.92;      % Efficienza DC-DC [92%]
pack. SOC_threshold = 0.02; % Soglia attivazione [5%]
pack. V_min_bal = 3.3;      % Tensione minima [V]
pack.V_max_bal = 4.15;     % Tensione massima [V]

batt_param = Simulink.Parameter;
batt_param.Value = pack;
batt_param.StorageClass = 'SimulinkGlobal';


sim_duration = 7000; % [s]
dt = 1;              % [s]
time_vector = (0:dt:sim_duration-dt)';

current_profile = zeros(size(time_vector));

cycle_length = 1000;   % [s]
n_cycles = sim_duration / cycle_length;

for c = 0:n_cycles-1
    idx = c * cycle_length;
    
    current_profile(idx + (101:300)) =  4;   % scarica
    current_profile(idx + (401:650)) = -3;   % carica
    current_profile(idx + (751:900)) =  2;   % scarica leggera
end

current_input_simulink = [time_vector, current_profile];


fprintf('Inizializzazione completata. Le variabili globali e i parametri del pacco sono pronti.\n');