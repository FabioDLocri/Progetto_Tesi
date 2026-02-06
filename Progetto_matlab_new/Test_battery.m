%Script generale che prima stima i parametri dell'ECM dal modello SPM, poi
%calcola le tensioni e correnti uscenti da 4 celle SPM con ingresso un
%profilo di potenza. Infine si inizializzano i parametri da mandare
%all'AEKF, per la stima di SOC e Capacità

clear; clc;

Hppc_test;

Voltage_test;

%Inizializziamo i valori di una singola cella
cell.nominal_voltage = 3.34; %V
cell.capacity = 2.3;   %Ah, della batteria originale caratterizzata
cell.ocv = parameter_discharge.OCV';
cell.soc = parameter_discharge.SOC';
cell.r0 = parameter_discharge.R0';
cell.r1 = parameter_discharge.R1';
cell.c1 = parameter_discharge.C1';

%%%%%%%%%%%%%%% parametri delle celle %%%%%%%%%%%%%%%%%%%%
Ncell=4;         %
c_nom=2.3;        % [Ah]
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%inizializziamo i parametri dell'intero pacco batteria in funzione di
%quelle della singola cella
batt.ncell = Ncell;
batt.c_nom = c_nom;
batt.c_cell = [2.3 2.3 2.3 2.3]';
batt.r0 = (cell.r0/cell.capacity) .* batt.c_cell;       %scalo i risultati per la capacità che considero
batt.r1= (cell.r1/cell.capacity) .* batt.c_cell;
batt.c1= (cell.c1/cell.capacity) .* batt.c_cell;
batt.soc=cell.soc;
batt.ocv=cell.ocv;
batt.soc0 = 1 + zeros(Ncell,1);

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
batt.docv=dOCV_tab;

%salviamo i parametri su un simulink parameter
batt_param = Simulink.Parameter;
batt_param.Value = batt;
batt_param.StorageClass = 'SimulinkGlobal';

simIn = Simulink.SimulationInput("Test_Aekf");
%Prelevo le uscite
out = sim(simIn);

SOC_ext = out.get('SOC_ext');
SOC_error = out.get('SOC_err');
Capacity_mis = out.get("Capacity_mis");
Capacity_err=out.get("Capacity_err");

    
    %plottiamo i parametri del SOC
    figure('Name',sprintf('Q celle %4.2f',batt.c_cell),'NumberTitle','off');
    
    subplot(2,1,1);
    plot(squeeze(SOC_ext.Time), squeeze(SOC_ext.Data)*100);
    xlabel('Time (s)');
    ylabel('State of Charge Ext (%)');
    title(sprintf('SOC EXT simulazione %d',k));
    
    
    subplot(2,1,2);
    plot(squeeze(SOC_error.Time), (squeeze(SOC_error.Data)*100));
    xlabel('Time (s)');
    ylabel('State of Charge error(%)');
    title(sprintf('SOC Error simulazione: %d',k));

    %Plottiamo i risultati della capacità
    
    figure('Name',sprintf('Q celle %4.2f',batt.c_cell),'NumberTitle','off');

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

