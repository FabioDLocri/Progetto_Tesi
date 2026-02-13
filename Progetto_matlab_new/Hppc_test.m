    
    M1_SPM_param_from_Bertei;
    
    model = "Parameter_estimation";
    cellname='Cell';
    M2_apply_param_to_model;
    %% PCT definition

    C_cell = 55.84; %Ah
    Pct_profile.N_pulses = 100;
    Pct_profile.I_pulse = C_cell;      % 1-Crate
    Pct_profile.Q_pulse = C_cell/Pct_profile.N_pulses;
    Pct_profile.t_pulse = abs(3600*Pct_profile.Q_pulse/Pct_profile.I_pulse);
    Pct_profile.t_relax = Pct_profile.t_pulse*5;      % s

    Pct_profile.Ts = 1;

    Pct_profile.I_ch_ts = generate_PCT_profile_f(-Pct_profile.I_pulse, Pct_profile.t_pulse, Pct_profile.t_relax, Pct_profile.N_pulses, Pct_profile.Ts);
    Pct_profile.I_disch_ts = generate_PCT_profile_f( Pct_profile.I_pulse, Pct_profile.t_pulse, Pct_profile.t_relax, Pct_profile.N_pulses, Pct_profile.Ts);
    %% Discharge mode
    set_param(strcat(model,'/','SoC_ThrMode'),'Operator','<=')
    set_param(strcat(model,'/','SoC_ThrVal'),'Value','0.001')
    set_param(strcat(model,'/','Cell'),'stateOfCharge',"1");
    I_source=Pct_profile.I_disch_ts;

    simIn = Simulink.SimulationInput("Parameter_estimation");
    out = sim(simIn);
    
    Hppc_discharge.v = out.get('V_cell');
    Hppc_discharge.i = out.get('I_cell');
    
    %% Charge mode
    
    I_source=Pct_profile.I_ch_ts;
    
    set_param(strcat(model,'/','SoC_ThrMode'),'Operator','>=')
    set_param(strcat(model,'/','SoC_ThrVal'),'Value','0.999')
    set_param(strcat(model,'/','Cell'),'stateOfCharge',"0");  
    
    simIn = Simulink.SimulationInput("Parameter_estimation");
    out = sim(simIn);

    Hppc_charge.v = out.get('V_cell');
    Hppc_charge.i = out.get('I_cell');
 %%
    %di default la carica è positiva, mentre la scarica è negativa
    t_offset = Hppc_discharge.v.Time(end);
    hppc.t = [Hppc_discharge.v.Time(:);Hppc_charge.v.Time(:)+t_offset];
    hppc.v = [Hppc_discharge.v.Data(:);Hppc_charge.v.Data(:)];
    hppc.i = [Hppc_discharge.i.Data(:);Hppc_charge.i.Data(:)];

    SOCbreakpoint=linspace(0,1,21);
    equivalentCircuitModel = ecm(1); %creo il modello quivalente costituito da un gruppo RC
    equivalentCircuitModel.SOCBreakpoints=SOCbreakpoint;
    equivalentCircuitModel.ResistanceSOCBreakpoints=SOCbreakpoint;
    
    Hppcdata = table( hppc.t,  hppc.v,  hppc.i, 'VariableNames', {'Time','Voltage','Current'});
   
    
    hppcsim = hppcTest(Hppcdata, TimeVariable="Time", VoltageVariable="Voltage", CurrentVariable="Current",ValidPulseDurationRange=[2,Pct_profile.t_pulse*2],CurrentSignConvention="negativeDischarge");  
    
    batteryEcm = fitECM(hppcsim, SegmentToFit="loadAndRelaxation",ECM=equivalentCircuitModel);
 %%   
    %estraggo i parametri dalla tabella dei risultati

    parameter_discharge.R0=batteryEcm.ModelParameterTables.DischargeR0;
    parameter_discharge.R1=batteryEcm.ModelParameterTables.DischargeR1;
    parameter_discharge.C1=batteryEcm.ModelParameterTables.DischargeC1;
    parameter_discharge.SOC=SOCbreakpoint;
    parameter_discharge.OCV=batteryEcm.ModelParameterTables.DischargeOpenCircuitVoltage;

    parameter_charge.R0=batteryEcm.ModelParameterTables.ChargeR0;
    parameter_charge.R1=batteryEcm.ModelParameterTables.ChargeR1;
    parameter_charge.C1=batteryEcm.ModelParameterTables.ChargeC1;
    parameter_charge.SOC=SOCbreakpoint;
    parameter_charge.OCV=batteryEcm.ModelParameterTables.ChargeOpenCircuitVoltage;

    save('data/Parameter.mat', "parameter_discharge", "parameter_charge");
