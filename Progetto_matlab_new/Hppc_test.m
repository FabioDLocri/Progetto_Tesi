
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
    
    
    hppcsim = hppcTest(Hppcdata, TimeVariable="Time", VoltageVariable="Voltage", CurrentVariable="Current",ValidPulseDurationRange=[2,120],CurrentSignConvention="positiveDischarge");
    removePulse(hppcsim,104)
%%  
    batteryEcm = fitECM(hppcsim, SegmentToFit="loadAndRelaxation",ECM=equivalentCircuitModel);
    
    %estraggo i parametri dalla tabella dei risultati

    ParamSum_discharge=batteryEcm.ParameterSummary(batteryEcm.ParameterSummary.Directionality=="Discharge",:);
    ParamSum_charge=batteryEcm.ParameterSummary(batteryEcm.ParameterSummary.Directionality=="Charge",:);
    ParamSum_discharge_sorted = sortrows(ParamSum_discharge, 'SOC','descend');
    ParamSum_charge_sorted = sortrows(ParamSum_charge, 'SOC','descend');

    parameter_discharge.R0=table2array(ParamSum_discharge_sorted(1:end,7));
    parameter_discharge.R1=table2array(ParamSum_discharge_sorted(1:end,8));
    parameter_discharge.C1=table2array(ParamSum_discharge_sorted(1:end,10));
    parameter_discharge.SOC=table2array(ParamSum_discharge_sorted(1:end,3));
    parameter_discharge.OCV=table2array(ParamSum_discharge_sorted(1:end,6));

    parameter_charge.R0=table2array(ParamSum_charge_sorted(1:end,7));
    parameter_charge.R1=table2array(ParamSum_charge_sorted(1:end,8));
    parameter_charge.C1=table2array(ParamSum_charge_sorted(1:end,10));
    parameter_charge.SOC=table2array(ParamSum_charge_sorted(1:end,3));
    parameter_charge.OCV=table2array(ParamSum_charge_sorted(1:end,6));

    save('data/Parameter.mat', "parameter_discharge", "parameter_charge");
