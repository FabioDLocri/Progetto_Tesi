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

for k=1:4
    
    %%%%%%%%%%%%%%% parametri delle celle %%%%%%%%%%%%%%%%%%%%
    Ncell=4;         %
    %c_nom=8;        % [Ah] il fattore 1/3600 per convertire da [As] lo considero quando integro la corrente per calcolare il SoC in simulink per
    c_var=0.2;
    batt.soc0 = 1 + zeros(Ncell,1);
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




switch k
    case 1
            c_nom=2;
            c_min=c_nom*(1-c_var);
            c_max=c_nom*(1);
            batt.c_cell = (c_max:-(c_max-c_min)/(Ncell-1):c_min)';
    case 2
            c_nom=8;
            batt.c_cell = [7.2 7.6 8 7.7]';
    case 3
            c_nom=6;
            batt.c_cell = [6.3 6 4.5 5.5]';
    case 4
            c_nom=2;
            batt.c_cell = [2 1.9 1.8 1.7]';
end


    batt.ncell = Ncell;
    batt.c_nom = c_nom;
    batt.c_var = c_var;
    %batt.c_cell = 2*[4 3.2 3.8 3.6]';% (c_max:-(c_max-c_min)/(Ncell-1):c_min)';
    batt.r0 = (cell.r0/cell.capacity) .* batt.c_cell;       %scalo i risultati per la capacità che considero
    batt.r1= (cell.r1/cell.capacity) .* batt.c_cell;
    batt.c1= (cell.c1/cell.capacity) .* batt.c_cell;
    batt.soc=cell.soc;
    batt.ocv=cell.ocv;
    batt.soc0 = 1 + zeros(Ncell,1);

    %uso gli stessi parametri precedenti, ma facendo in modo che l'AEKF non
    %sappia la Capacità vera delle celle.
    pack.ncell = Ncell;
    pack.c_nom = batt.c_nom; %c_nom;
    pack.c_cell=([1 1 1 1].*c_nom)';
    pack.r0=cell.r0/cell.capacity .* pack.c_cell;
    pack.r1=cell.r1/cell.capacity .* pack.c_cell;
    pack.c1=cell.c1/cell.capacity .* pack.c_cell;
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


    sim_duration = 14000; % [s]
    durata_totale=4*sim_duration;
    dt = 1;              % [s]
    time_vector = (0:dt:durata_totale-dt)';

    current_profile = zeros(size(time_vector));

    cycle_length = 1000;   % [s]
    n_cycles = sim_duration / cycle_length;

    for c = 0:n_cycles-1
        idx = c * cycle_length;
    
        current_profile(idx + (101:300)) =  4;   % scarica
        current_profile(idx + (401:650)) = -3;   % carica
        current_profile(idx + (751:900)) =  2;   % scarica leggera
    
        current_profile(idx + sim_duration+ (101:300)) =  -4;   % carica
        current_profile(idx + sim_duration+(401:650)) = 3;   % scarica
        current_profile(idx + sim_duration+(751:850)) =  -2;   % carica leggera

        current_profile(idx + 2*sim_duration+(101:300)) =  3;   % scarica
        current_profile(idx + 2*sim_duration+(401:550)) = -3.5;   % carica
        current_profile(idx + 2*sim_duration+(751:900)) =  1.5;   % scarica leggera

        current_profile(idx + 3*sim_duration+(101:300)) =  -3;   % scarica
        current_profile(idx + 3*sim_duration+(401:550)) = 3.5;   % carica
        current_profile(idx + 3*sim_duration+(751:900)) =  -1.5;   % scarica leggera


    end
    current_input_simulink = [time_vector, current_profile];


    fprintf('Inizializzazione completata. Le variabili globali e i parametri del pacco sono pronti.\n');

    fprintf('Esecuzione Simulazione Simulink n: %d \n',k);

    simIn = Simulink.SimulationInput("BMS_SOC");
    out(k) = sim(simIn);
    
    SOC_true = out(k).get('SOC_true');
    SOC_mis = out(k).get('SOC_mis');
    SOC_error = out(k).get('SOC_err');
    Capacity_mis = out(k).get("Capacity_mis");

    figure;
    % Plotting the true and measured SOC for comparison
    subplot(3,1,1);
    plot(SOC_true.Time, SOC_true.Data);
    xlabel('Time (s)');
    ylabel('State of Charge (SOC)');
    title('SOC_True cella %d',k);
    
    subplot(3,1,2);
    plot(squeeze(SOC_mis.Time), squeeze(SOC_mis.Data));
    xlabel('Time (s)');
    ylabel('State of Charge Ext(SOC)');
    title('SOC EXT cella %d',k);
    
    subplot(3,1,3);
    plot(squeeze(SOC_err.Time), squeeze(SOC_err.Data));
    xlabel('Time (s)');
    ylabel('State of Charge error)');
    title('SOC Error cella: %d',k);

end


