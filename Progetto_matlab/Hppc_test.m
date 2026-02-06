    clear; clc;
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
        
%%
   simIn = Simulink.SimulationInput("Parameter_estimation");
    out = sim(simIn);
    
    Hppc_discharge.v = out.get('voltage_hppc_discharge');
    Hppc_discharge.i = out.get('current_hppc_discharge');

    Hppc_charge.v = out.get('voltage_hppc_charge');
    Hppc_charge.i = out.get('current_hppc_charge');
    
    %di default la carica è positiva, mentre la scarica è negativa
    t = [Hppc_discharge.v.Time(:);Hppc_charge.v.Time(:)+206058];
    v = [Hppc_discharge.v.Data(:);Hppc_charge.v.Data(:)];
    i = [Hppc_discharge.i.Data(:)*(-1);Hppc_charge.i.Data(:)*(-1)]; %moltiplico per -1, per far riconoscere carica come positiva e scarica come negativa

    equivalentCircuitModel = ecm(1); %creo il modello quivalente costituito da un gruppo RC

    Hppcdata = table(t, v, i, 'VariableNames', {'Time','Voltage','Current'});
    save('HppcData.mat', 'Hppcdata');
    
    hppcsim = hppcTest(Hppcdata, TimeVariable="Time", VoltageVariable="Voltage", CurrentVariable="Current",ValidPulseDurationRange=[2,120],CurrentSignConvention="positiveDischarge");
    removePulse(hppcsim,104)
%%
    batteryEcm = fitECM(hppcsim, SegmentToFit="loadAndRelaxation",ECM=equivalentCircuitModel);
    
    %estraggo i parametri dalla tabella dei risultati
    parameter_discharge.R0=table2array(batteryEcm.ParameterSummary(1:103,7));
    parameter_discharge.R1=table2array(batteryEcm.ParameterSummary(1:103,8));
    parameter_discharge.C1=table2array(batteryEcm.ParameterSummary(1:103,10));
    parameter_discharge.SOC=table2array(batteryEcm.ParameterSummary(1:103,3));
    parameter_discharge.OCV=table2array(batteryEcm.ParameterSummary(1:103,6));

    parameter_charge.R0=table2array(batteryEcm.ParameterSummary(104:end,7));
    parameter_charge.R1=table2array(batteryEcm.ParameterSummary(104:end,8));
    parameter_charge.C1=table2array(batteryEcm.ParameterSummary(104:end,10));
    parameter_charge.SOC=table2array(batteryEcm.ParameterSummary(104:end,3));
    parameter_charge.OCV=table2array(batteryEcm.ParameterSummary(104:end,6));
