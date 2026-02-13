    %Carico il profilo di potenza che mi interessa per testare le celle ed
    %estraggo tensione e corrente da Simulink
    Ncell=4;

    load('data/input/load_profile_raw.mat');    %[kW]
    load('data/input/PV_irr_raw.mat');          %[W]
    
    pv_profile_raw.date_time = datetime( pv_profile_raw.date_time,'InputFormat', 'yyyy-MM-dd HH:mm:ss');

    pv_profile_raw_interp.t = (pv_profile_raw.date_time(1):minutes(1):pv_profile_raw.date_time(end))';
    pv_profile_raw_interp.I= interp1(pv_profile_raw.date_time, pv_profile_raw.I, pv_profile_raw_interp.t, 'linear');

    pv_pk = 10;
    start_day = 22;   % giorno dell'anno
    n_days    = 0.5;  % durata simulazione

 
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



    
    %% Applico i parametri del modello SPM alle rispettive celle
    model = "Voltage_estimation";
    for k=1:Ncell
        cellname = sprintf('Cell%d', k);
        M2_apply_param_to_model;
    end
    %% Estraiamo da simulink i valori di tensione e corrente dalle celle
    simIn = Simulink.SimulationInput("Voltage_estimation");
    out = sim(simIn);
%%
    V_cell=out.get("V_cell");
    I_cell=out.get("I_cell")*(-1);
    SOC_celle_mis=out.get("SOC_cella_mis");
    V_pack=out.get("V_pack");

    save('data/V_I_SOC_cell.mat', "V_cell", "I_cell","SOC_celle_mis","V_pack");