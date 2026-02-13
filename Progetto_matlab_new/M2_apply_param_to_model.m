%% ASSEGNO PARAMETRI A MODELLO SIMULINK
% assegno tutti i parametri che avevo definito prima nel workspace al
% modello simulink.
% NB: chiamare questo script prima di lanciare simulazione
load_system(model)
%%%%%%%%%% geometry 
set_param(strcat(model,'/',cellname), 'ThicknessAnode', string(SPM.geom.Anode_thickness));
set_param(strcat(model,'/',cellname), 'ThicknessSeparator', string(SPM.geom.Separator_thickness));
set_param(strcat(model,'/',cellname), 'ThicknessCathode', string(SPM.geom.Cathode_thickness));
set_param(strcat(model,'/',cellname), 'ElectrodePlateArea', string(SPM.geom.Electrode_plate_area));
set_param(strcat(model,'/',cellname), 'ParticleRadiusAnode', string(SPM.geom.Anode_particle_radius));
set_param(strcat(model,'/',cellname), 'ParticleRadiusCathode', string(SPM.geom.Cathode_particle_radius));

%%%%%%%%%% electrode properties
set_param(strcat(model,'/',cellname),'ActiveMaterialVolumeFractionAnode',string(SPM.electrode.Volume_fraction_of_anode_active_material));
set_param(strcat(model,'/',cellname),'ActiveMaterialVolumeFractionCathode',string(SPM.electrode.Volume_fraction_of_cathode_active_material));
set_param(strcat(model,'/',cellname),'OpenCircuitPotentialAnode',mat2str(SPM.electrode.Anode_OCP_relative_to_absolute_stoichiometry));
set_param(strcat(model,'/',cellname),'OpenCircuitPotentialCathode',mat2str(SPM.electrode.Cathode_OCP_relative_to_absolute_stoichiometry));
set_param(strcat(model,'/',cellname),'NormalizedStoichiometryBreakpoints',mat2str(theta_abs));
set_param(strcat(model,'/',cellname),'MaximumConcentrationAnode',string(SPM.electrode.Anode_maximum_ion_concentration));
set_param(strcat(model,'/',cellname),'MaximumStoichiometryAnode',string(SPM.electrode.Anode_maximum_stoichiometry));
set_param(strcat(model,'/',cellname),'MinimumStoichiometryAnode',string(SPM.electrode.Anode_minimum_stoichiometry));
set_param(strcat(model,'/',cellname),'MaximumConcentrationCathode',string(SPM.electrode.Cathode_maximum_ion_concentration));
set_param(strcat(model,'/',cellname),'MaximumStoichiometryCathode',string(SPM.electrode.Cathode_maximum_stoichiometry));
set_param(strcat(model,'/',cellname),'MinimumStoichiometryCathode',string(SPM.electrode.Cathode_minimum_stoichiometry));
set_param(strcat(model,'/',cellname),'ActiveMaterialDiffusionCoefficientAnode',string(SPM.electrode.Diffusion_coefficient_of_anode_active_material));
set_param(strcat(model,'/',cellname),'ActiveMaterialDiffusionCoefficientCathode',string(SPM.electrode.Diffusion_coefficient_of_cathode_active_material));
set_param(strcat(model,'/',cellname),'ConductivityAnode',string(SPM.electrode.Anode_conductivity));
set_param(strcat(model,'/',cellname),'ConductivityCathode',string(SPM.electrode.Cathode_conductivity));
set_param(strcat(model,'/',cellname),'CurrentCollectorResistance',string(SPM.electrode.Current_collector_resistance));

%%%%%%%%%% electrolyte properties
set_param(strcat(model,'/',cellname),'ElectrolyteConductivity',string(SPM.electrolyte.Electrolyte_conductivity));
set_param(strcat(model,'/',cellname),'ElectrolyteVolumeFractionAnode',string(SPM.electrolyte.Volume_fraction_of_electrolyte_in_anode));
set_param(strcat(model,'/',cellname),'ElectrolyteVolumeFractionSeparator',string(SPM.electrolyte.Volume_fraction_of_electrolyte_in_separator));
set_param(strcat(model,'/',cellname),'ElectrolyteVolumeFractionCathode',string(SPM.electrolyte.Volume_fraction_of_electrolyte_in_cathode));
set_param(strcat(model,'/',cellname),'BruggemanExponentAnode',string(SPM.electrolyte.Anode_Bruggeman_exponent));
set_param(strcat(model,'/',cellname),'BruggemanExponentSeparator',string(SPM.electrolyte.Separator_Bruggeman_exponent));
set_param(strcat(model,'/',cellname),'BruggemanExponentCathode',string(SPM.electrolyte.Cathode_Bruggeman_exponent));
set_param(strcat(model,'/',cellname),'TransferenceNumber',string(SPM.electrolyte.Transference_number));
set_param(strcat(model,'/',cellname),'ElectrolyteDiffusionCoefficient',string(SPM.electrolyte.Diffusion_coefficient_of_electrolyte));
%%%%%%%%%%% thermal properties
set_param(strcat(model,'/',cellname),'ActiveMaterialDiffusionActivationEnergyAnode',string(SPM.thermal.Activation_energy_for_diffusion_in_anode_active_material));
set_param(strcat(model,'/',cellname),'ActiveMaterialDiffusionActivationEnergyCathode',string(SPM.thermal.Activation_energy_for_diffusion_in_cathode_active_material));
set_param(strcat(model,'/',cellname),'ChargeTransferActivationEnergyAnode',string(SPM.thermal.Activation_energy_for_charge_transfer_in_Anode));
set_param(strcat(model,'/',cellname),'ChargeTransferActivationEnergyCathode',string(SPM.thermal.Activation_energy_for_charge_transfer_in_Cathode));