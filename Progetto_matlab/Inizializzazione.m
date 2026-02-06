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
%c_nom=8;        % [Ah] il fattore 1/3600 per convertire da [As] lo considero quando integro la corrente per calcolare il SoC in simulink per
c_var=0.2;
batt.soc0 = 1 + zeros(Ncell,1);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

for k=2:2
    
switch k
    case 1
            c_nom=2;
            c_min=c_nom*(1-c_var);
            c_max=c_nom*(1);
            batt.c_cell = (c_max:-(c_max-c_min)/(Ncell-1):c_min)';
            pack.sigma_v = 0;
            pack.sigma_i = 0;
            pack.offset_v = 0;
            pack.offset_i = 0;
    case 2
            c_nom=20;
            batt.c_cell = [20 19.9 19.8 17]';
            pack.sigma_v = 0;
            pack.sigma_i = 0;
            pack.offset_v = 0;
            pack.offset_i = 0;
    case 3
            c_nom=2;
            c_min=c_nom*(1-c_var);
            c_max=c_nom*(1);
            batt.c_cell = (c_max:-(c_max-c_min)/(Ncell-1):c_min)';
            pack.sigma_v = 0.000001;
            pack.sigma_i = 0.000001;
            pack.offset_v = 0.001;
            pack.offset_i = 0.001;
    case 4
            c_nom=20;
            batt.c_cell = [20 19.9 19.8 17]';
            pack.sigma_v = 0.000001;
            pack.sigma_i = 0.000001;
            pack.offset_v = 0.001;
            pack.offset_i = 0.001;
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
    
    %calcoliamo la tabella delle derivata
    SOC_grid =cell.soc;
    OCV_tab  =cell.ocv;
    
    n_points = length(SOC_grid);
    dSOC = SOC_grid(2) - SOC_grid(1);
    dOCV_temp = diff(OCV_tab) / dSOC;
    dOCV_tab = zeros(n_points, 1);
    for i = 1:n_points-1
        dOCV_tab(i) = dOCV_temp(i);
    end
    dOCV_tab(n_points) = dOCV_temp(n_points-1);
    pack.docv=dOCV_tab;

    % Parametri Bilanciamento DC-DC

    pack.I_bal_max = 1;      % Corrente max bilanciamento [A]
    pack.eta_dcdc = 0.92;      % Efficienza DC-DC [92%]
    pack. SOC_threshold = 0.02; % Soglia attivazione [5%]
    pack. V_min_bal = 3.3;      % Tensione minima [V]
    pack.V_max_bal = 4.15;     % Tensione massima [V]

    batt_param = Simulink.Parameter;
    batt_param.Value = pack;
    batt_param.StorageClass = 'SimulinkGlobal';


    %-------------
    load('data/input/load_profile_raw.mat');    %[kW]
    load('data/input/PV_irr_raw.mat');          %[W]

    pv_profile_raw.date_time = datetime( pv_profile_raw.date_time,'InputFormat', 'yyyy-MM-dd HH:mm:ss');

    pv_profile_raw_interp.t = (pv_profile_raw.date_time(1):minutes(1):pv_profile_raw.date_time(end))';
    pv_profile_raw_interp.I= interp1(pv_profile_raw.date_time, pv_profile_raw.I, pv_profile_raw_interp.t, 'linear');

    pv_pk = 10;
    start_day = 48;   % giorno dell'anno
    n_days    = 5;  % durata simulazione

 
    t0 = pv_profile_raw_interp.t(1) + days(start_day - 1);
    t1 = t0 + days(n_days) - minutes(1);

    t2 = load_profile_raw.t(1) + days(start_day - 1);
    t3 = t2 + days(n_days) - minutes(1);


    idx1 = pv_profile_raw_interp.t >= t0 & pv_profile_raw_interp.t <= t1;
    t_pvprofile = pv_profile_raw_interp.t(idx1);
    i_pvprofile = pv_profile_raw_interp.I(idx1)*pv_pk *-1;
    t_sec_pvprofile = seconds(t_pvprofile - t_pvprofile(1));
    pv_profile = [t_sec_pvprofile, i_pvprofile];

    StopTime = t_sec_pvprofile(end);

    idx2 = load_profile_raw.t >= t2 & load_profile_raw.t <= t3;
    t_loadprofile = load_profile_raw.t(idx2);
    i_loadprofile = load_profile_raw.P(idx2)* 1e3;
    t_sec_loadprofile   = seconds(t_loadprofile - t_loadprofile(1));
    load_profile = [t_sec_loadprofile, i_loadprofile];

    %---------------------------

%%

    fprintf('Inizializzazione completata. Le variabili globali e i parametri del pacco sono pronti.\n');

    fprintf('Esecuzione Simulazione Simulink n: %d \n',k);

    simIn = Simulink.SimulationInput("BMS_SOC");
    out = sim(simIn);
    
    SOC_true = out.get('SOC_true');
    SOC_mis = out.get('SOC_mis');
    SOC_error = out.get('SOC_err');
    Capacity_mis = out.get("Capacity_mis");
    Capacity_err=out.get("Capacity_err");
    V_cell=out.get("V_cell");
    I_cell=out.get("I_cell");
    V_cell_noisy=out.get("V_cell_noisy");
    I_cell_noisy=out.get("I_cell_noisy");

    %plottiamo i parametri del SOC
    figure('Name',sprintf('Q celle %4.2f',batt.c_cell),'NumberTitle','off');
    
    subplot(2,1,1);
    plot(squeeze(SOC_mis.Time), squeeze(SOC_mis.Data)*100);
    xlabel('Time (s)');
    ylabel('State of Charge Ext (%)');
    title(sprintf('SOC EXT simulazione %d',k));
    
    
    subplot(2,1,2);
    plot(squeeze(SOC_error.Time), (squeeze(SOC_error.Data)*100));
    xlabel('Time (s)');
    ylabel('State of Charge error(%)');
    title(sprintf('SOC Error simulazione: %d',k));

    %Plottiamo i risultati della capacità
    % N=length(Capacity_mis.Time);
    % V=repmat(batt.c_cell,1,N);
    % 
    figure('Name',sprintf('Q celle %4.2f',batt.c_cell),'NumberTitle','off');
    % subplot(3,1,1);
    % plot(squeeze(Capacity_mis.Time), squeeze(V));
    % xlabel('Time (s)');
    % ylabel('Capacità (Ah)');
    % title(sprintf('Capacità simulazione %d',k));
    
    subplot(2,1,1);
    plot(squeeze(Capacity_mis.Time), squeeze(Capacity_mis.Data));
    xlabel('Time (s)');
    ylabel('Capacity Ext (Ah)');
    title(sprintf('Capacity EXT simulazione %d',k));
    

    Capacity_err_perc=squeeze(Capacity_err.Data)./batt.c_cell*100;
    subplot(2,1,2);
    plot(squeeze(Capacity_err.Time), Capacity_err_perc);
    xlabel('Time (s)');
    ylabel('Capacity error (%)'); 
    title(sprintf('Capacity Error simulazione: %d',k));

    %plottiamo i valori prima e dopo l'inserimento del rumore
    figure('Name',sprintf('Tensione celle'),'NumberTitle','off');
    
    subplot(2,1,1);
    plot(squeeze(V_cell.Time), squeeze(V_cell.Data));
    xlabel('Time (s)');
    ylabel('V cell noiseless(V)');
    title(sprintf('Tensione celle simulazione %d',k));
    
    subplot(2,1,2);
    plot(squeeze(V_cell_noisy.Time), squeeze(V_cell_noisy.Data));
    xlabel('Time (s)');
    ylabel('V cell noisy(V)');
    title(sprintf('Tensione celle con rumore simulazione: %d',k));

    figure('Name',sprintf('Corrente celle'),'NumberTitle','off');
    
    subplot(2,1,1);
    plot(squeeze(I_cell.Time), squeeze(I_cell.Data));
    xlabel('Time (s)');
    ylabel('I cell noiseless(A)');
    title(sprintf('Corrente celle simulazione %d',k));
    
    subplot(2,1,2);
    plot(squeeze(I_cell_noisy.Time), squeeze(I_cell_noisy.Data));
    xlabel('Time (s)');
    ylabel('I cell noisy(A)');
    title(sprintf('Corrente celle con rumore simulazione: %d',k));

end
