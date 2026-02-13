% partendo dall'articolo di bertei, vado a ricavare i parametri da usare
% nel modello SPM di simulink, per una cella NMC333 da 54.71 Ah*m^-2 
    % la cella è descritta normalizzata rispetto all'area degli elettrodi,
    % così variando la superficie ottengo la cella delle dimensioni che
    % voglio.
        % NB: la capacità della cella è data dal volume! V=area*thickness,
        % e poi del volume posso usare solo la frazione dell'area attiva

% https://pdf.sciencedirectassets.com/271355/1-s2.0-S0013468621X00266/1-s2.0-S0013468621013888/main.pdf

%% potenziali problemi
% la cathode conductivity calcolata secondo eq A5-6  è troppo grande,
% sembra che sia colpa del valore della conductivity del carbon black -->
% assumo 0.5 come il valore di default di simulink

%
%% parametri ausiliari definiti nel paper Bertei

%%%%%%%%%%%%%%%%       parametri ausiliari     %%%%%%%%%%%%%%%%
theta_max_p = 0.99;     % stochiometria (stato di litiazione) 
theta_min_p = 0.19;
theta_max_n = 0.97;
theta_min_n = 0.02;


eps_el       = 0.108; % (Bertei) volume fraction of conductive additive (electronic phase) [-]
eps_am       = 0.49; % (Bertei) volume fraction of active material (NMC) [-]
bruggeman_p       = 2.26; % (Bertei) Bruggeman / tortuosity factor of positive electrode [-]

T_ref = 25+273;% temperatura alla quale valuto i parametri
ocp_points = 100;
%%%%%%%%%%%%%%%%    conduttività elettrodo positivo     %%%%%%%%%%%%%%%%
% eq A5-6 (conduttività elettrodo positivo). E' definita al variare di
% T(assumo 25°=298K), e di sol_p che è lo stato di lithiazione(assumo il valore
% medio tra sol_max e sol_min)
T = T_ref; %[K]

theta = (theta_max_p + theta_min_p)/2; % stato di litiazione del catodo (0–1)
% Parameters:
sigma_e_carb = 1e4;  % (MM), non c'era nel paper. electrical conductivity of carbon black [S/m]


% test per vedere variabilità
%theta = linspace(sol_p_min,sol_p_max, 30);
%T = linspace(273, 283, 30); %sigma_e_NMC sembra che varia linearmente su T

% eq. A5-6 trascritta da gpt, dal polinomio del paper
sigma_e_NMC = ...
    0.09741 ...
  - 1.635*theta ...
  - 4.731e-5*T ...
  + 0.5873*theta.^2 ...
  + 0.007729*theta.*T ...
  - 3.87e-15*T.^2 ...
  + 8.679*theta.^3 ...
  - 0.01034*theta.^2.*T ...
  - 1.33e-15*theta.*T.^2 ...
  + 1.32e-17*T.^3 ...
  - 26.27*theta.^4 ...
  + 5.21e-5*theta.^3.*T ...
  + 8.52e-17*theta.^2.*T.^2 ...
  + 2.86e-18*theta.*T.^3 ...
  - 2.24e-20*T.^4 ...
  + 12.36*theta.^5 ...
  + 0.03499*theta.^4.*T ...
  + 7.14e-18*theta.^3.*T.^2 ...
  - 1.05e-19*theta.^2.*T.^3 ...
  - 2.29e-21*theta.*T.^4 ...
  + 1.52e-23*T.^5;

sigma_e_P = ...
    ( sigma_e_carb*eps_el + sigma_e_NMC*eps_am ) ...
    ./ ( (eps_el + eps_am) .^ (1 - bruggeman_p) );

%%%%%%%%%%%%%%%%    diffusività elettrodo negativo     %%%%%%%%%%%%%%%%
% Negative electrode solid diffusivity vs stoichiometry
% SoL_N  : anode stoichiometry [-]
% Ds_N0  : solid-phase diffusivity [10^-13 m^2/s]

SoL_N = [ ...
    0.22 0.25 0.32 0.37 0.38 0.41 0.45 0.50 0.52 0.56 0.58 ...
    0.63 0.67 0.68 0.70 0.71 0.74 0.79 0.87 0.87 0.89 ];

Ds_N0 = [ ...
    9.87 8.12 4.14 2.16 1.90 1.17 0.68 0.25 5.80 4.14 3.78 ...
    2.63 2.22 2.11 1.97 1.83 1.62 1.50 1.00 0.82 0.70 ] * 1e-13;

%%%%%%%%%%%%%%%%    diffusività elettrodo positivo     %%%%%%%%%%%%%%%%
% Positive electrode solid diffusivity vs stoichiometry
% SoL_P  : cathode stoichiometry [-]
% Ds_P0  : solid-phase diffusivity [10^-14 m^2/s]

SoL_P = [ ...
    0.10 0.15 0.20 0.25 0.30 0.35 0.40 0.45 0.50 0.55 ...
    0.60 0.65 0.70 0.75 ];

Ds_P0 = [ ...
    4.07 2.26 2.20 1.82 1.87 1.67 1.57 1.48 1.08 0.90 ...
    1.00 1.07 1.10 1.29 ] * 1e-14;

%%%%%%%%%%%%%%%%    OCV elettrodo negativo          %%%%%%%%%%%%%%%%
% SoL_N  : anode stoichiometry [-]
% T_ref  : reference temperature [K]
% R      : universal gas constant [J/(mol*K)]
% F      : Faraday constant [C/mol]

R = 8.314;           % [J/(mol*K)]
F = 96458.33;           % [C/mol]
theta_n = linspace(theta_min_n, theta_max_n, ocp_points);
theta_abs = linspace(0,1,ocp_points);
A = ( ...
    -40 .* exp(-theta_n./0.015) ...
    + 0.75 .* (tanh((theta_n-0.17)./0.02) - 1) ...
    + (tanh((theta_n-0.22)./0.04) - 1) ...
    ) .* 0.5 .* ( -tanh((theta_n-0.35)./0.05) + 1 );

B = -0.05 .* theta_n.^(-0.85);

C = 10 .* 0.5 .* ( tanh((theta_n-1)./0.045) + 1 );

D = 6.12 .* (0.4 - theta_n.^0.98) ...
    .* 0.5 .* ( -tanh((theta_n-0.49)./0.045) + 1 ) ...
    .* 0.5 .* ( tanh((theta_n-0.35)./0.05) + 1 );

E = (1.36 .* (0.74 - theta_n) + 1.26) ...
    .* 0.5 .* ( tanh((theta_n-0.5)./0.02) + 1 );

Y = R .* T_ref .* (0.18 + A + B + C + D + E);

OCP_N = 0.136 - Y ./ F;

%%%%%%%%%%%%%%%%    OCV elettrodo positivo          %%%%%%%%%%%%%%%%
% Positive electrode (NMC) reference open-circuit voltage
% theta : cathode stoichiometry vector [-]

theta_p = linspace(theta_min_p, theta_max_p, ocp_points);

OCP_P = ...
    7.9760 ...
  - 5.5419 .* theta_p ...
  + 5.2824 .* theta_p.^1.07 ...
  - 1.0556e-4 .* exp(124.7407 .* theta_p - 114.2593) ...
  - 4.0446 .* theta_p.^0.0766;

% figure;
% tiledlayout(2,1);
% nexttile(1); hold on;
% plot(theta_p,OCP_P); title("OCP_P (V)");
% nexttile(2); hold on;
% plot(theta_n,OCP_N); title("OCP_N (V)");

%%%%%%%%%%%%%%%%    conduttività elettrolita          %%%%%%%%%%%%%%%%
% Mean ionic conductivity of the electrolyte
% c : electrolyte concentration [mol/m^3] (scalar or vector)
% T : temperature [K] (scalar or vector)

c = 1200; % (Bertei)  [mol/m^3]  concentrazione elettrolita
sigma_electrolyte = c .* 1e-4 .* ( ...
    -10.5 ...
    + 0.074 .* T ...
    - 6.96e-5 .* T.^2 ...
    + 0.668e-3 .* c ...
    - 0.0178e-3 .* c .* T ...
    + 2.8e-8 .* c .* T.^2 ...
    + 0.494e-6 .* c.^2 ...
    - 8.86e-10 .* c.^2 .* T).^2 ;

%%%%%%%%%%%%%%%% diffusività elettrolita    %%%%%%%%%%%%%%%%
% Ambipolar electrolyte diffusivity (ambipolar perchè tiene conto della
% diffusività della coppia ione-anione
% c : electrolyte concentration [mol/m^3]
% T : temperature [K]

diff_electrolyte = 1e-4 .* 10 .^ ( ...
    -4.43 ...
    - 54 ./ (T - 0.005 .* c - 229) ...
    - 2.2e-4 .* c ...
    );

clear 'Y' 'F' 'A' 'B' 'C' 'D' 'E' 'R'
%% descrizione parametri SPM simulink 
% quelli commentati sono lasciati al valore di default di simulink (perchè
% non li ho trovati nel paper di bertei)

%%%%%%%%%% geometry %%%%%%%%%%%%%%

SPM.geom.Anode_thickness = 147e-6; %[m] (Bertei)
SPM.geom.Separator_thickness = 25e-6; % [m] (Bertei)
SPM.geom.Cathode_thickness = 100e-6; %[m] (Bertei)

SPM.geom.Electrode_plate_area = 1; % [m] (MM) NB: scelto da me per avere una cella da 54.71Ah

SPM.geom.Anode_particle_radius = 12e-6; % [m] (Bertei)
SPM.geom.Cathode_particle_radius = 5e-6; %[m] (Bertei)

% parametri per risoluzione numerica
SPM.geom.Electrolyte_layer_count_of_anode = 10;      % default simulink SPM è un parametro numerico per discretizzare, è il n. di strati della mesh considerati dentro all'anodo
SPM.geom.Electrolyte_layer_count_of_separator = 5;   % idem sopra
SPM.geom.Electrolyte_layer_count_of_cathode = 10;    % idem sopra
SPM.geom.Anode_shell_count = 10;                     %  parametro numerico per discretizzare il n. di gusci nella risoluzione delle eq. per la particella circolare
SPM.geom.Cathode_shell_count = 10;                   % idem sopra

%%%%%%%%%% electrode properties %%%%%%%%%%%%%%
SPM.electrode.Volume_fraction_of_anode_active_material = 0.466; % [-] (Bertei)
SPM.electrode.Volume_fraction_of_cathode_active_material = eps_am; % [-] (Bertei)

%SPM.electrode.Stoichiometry_breakpoints_specification = Absolute; % nel blocco SPM di simulink 2025a può definisce se la stochiometria è 'absolute' o 'normalized', in simulink 2024a invece è SOLO absolute

%SPM.electrode.Normalized_stoichiometry_breakpoints
%SPM.electrode.Anode_OCP_relative_to_normalized_stoichiometry
%SPM.electrode.Cathode_OCP_relative_to_normalized_stoichiometry

SPM.electrode.Absolute_anode_stoichiometry_breakpoints = theta_n;                       % MM, decisi valori e poi valutata la funzione di bertei 
SPM.electrode.Anode_OCP_relative_to_absolute_stoichiometry = OCP_N;  % (V) (bertei), da funzione interpolante
SPM.electrode.Absolute_cathode_stoichiometry_breakpoints = theta_p; 
SPM.electrode.Cathode_OCP_relative_to_absolute_stoichiometry = OCP_P;

% stato di lithiazione = stochiometria (θ) = C_s/C_smax
% θn​=θn_min​+SoC⋅(θn_max​−θn_min​) --> SoC = (θn- θn_min)/(θn_max-θn_min)  
SPM.electrode.Anode_maximum_ion_concentration   = 31850;        % [mol m^-3] (Bertei)
SPM.electrode.Anode_maximum_stoichiometry       = theta_max_n;         % [-] (Bertei). è lo stato di lithiazione massimo
SPM.electrode.Anode_minimum_stoichiometry       = theta_min_n;         % [-] (Bertei)

SPM.electrode.Cathode_maximum_ion_concentration = 52500;    % [mol m^-3] (Bertei)
SPM.electrode.Cathode_maximum_stoichiometry     = theta_max_p;     % [-] (Bertei) è lo stato di lithiazione massimo. sul catodo ce l'ho quando la cella è scarica. 
SPM.electrode.Cathode_minimum_stoichiometry     = theta_min_p;     % [-] (Bertei)
SPM.electrode.theta_abs                         = theta_abs;       % stato di lithiazione, mi serve per i breakpoint della tabella della stochiometria

SPM.electrode.Diffusion_coefficient_of_anode_active_material   = mean(Ds_N0); %  [m^2/s] (Bertei), prendo il valor medio, che Bertei riporta una tabella al variare della stochiometria
SPM.electrode.Diffusion_coefficient_of_cathode_active_material = mean(Ds_P0); % idem

SPM.electrode.Anode_conductivity                = 100; % [S m^-1] (Bertei)
SPM.electrode.Cathode_conductivity              = 0.5; % [S m^-1] (MM) il calcolo veniva un valore gigante, colpa del valore forse errato della carbon black conductivity
SPM.electrode.Current_collector_resistance      = (5.96e7^-1 + 3.55e7^-1); % [S m^-1] bertei definisce quella del collettore positivo e negativo: 5.96e7 e 3.55e7, essendo conduttanze le metto in parallelo. POI CONSIDERO LA RESISTENZA

%%%%%%%%%% electrolyte properties %%%%%%%%%%%%%%
SPM.electrolyte.Volume_fraction_of_electrolyte_in_anode     = 0.477;    % [-] (Bertei)
SPM.electrolyte.Volume_fraction_of_electrolyte_in_separator = 0.45;     % [-] (Bertei)
SPM.electrolyte.Volume_fraction_of_electrolyte_in_cathode   = 0.36;     % [-] (Bertei)

SPM.electrolyte.Electrolyte_conductivity                    = sigma_electrolyte; 

SPM.electrolyte.Anode_Bruggeman_exponent        = 1.86;         % [-] (Bertei)
SPM.electrolyte.Separator_Bruggeman_exponent    = 1.5;
SPM.electrolyte.Cathode_Bruggeman_exponent      = bruggeman_p;

SPM.electrolyte.Transference_number             = 0.38; % [-] (Bertei)
SPM.electrolyte.Diffusion_coefficient_of_electrolyte    = diff_electrolyte ;  % è la ambipolar electrolyte diffusivity che riporta Bertei

%%%%%%%%%% reaction kinetics %%%%%%%%%%%%%%
%SPM.reaction.Charge_transfer_rate_constant_for_Anode
%SPM.reaction.Charge_transfer_rate_constant_for_Cathode

%%%%%%%%%% thermal %%%%%%%%%%%%%%
%SPM.thermal.Battery_thermal_mass
SPM.thermal.Arrhenius_reference_temperature = 298.15; %[K] default

SPM.thermal.Activation_energy_for_diffusion_in_anode_active_material = 50000; % [J mol^-1] (Bertei)
SPM.thermal.Activation_energy_for_diffusion_in_cathode_active_material = 25000; 
%SPM.thermal.Activation_energy_for_diffusion_in_electrolyte ; % default

%SPM.thermal.Activation_energy_for_conduction_in_anode           =  ; % default% non serbra esserci nel paper. dovrebbe regolare con la legge di arrenius la temperatura nella cella.
%SPM.thermal.Activation_energy_for_conduction_in_cathode            ; % default
%SPM.thermal.Activation_energy_for_conduction_in_electrolyte        ; % default

SPM.thermal.Activation_energy_for_charge_transfer_in_Anode      = 30000; %(Bertei)
SPM.thermal.Activation_energy_for_charge_transfer_in_Cathode    = 30000; % (Bertei)


clearvars -except SPM theta_abs



