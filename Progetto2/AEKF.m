function SOC_out  = AEKF(V_meas, I_meas, reset,batt_param)  %#codegen

% Ingressi:
%   V_meas - Vettore tensioni misurate [n_cells x 1]
%   I_meas - Vettore correnti misurate [n_cells x 1]  
%   reset  - Segnale di reset (boolean)
%
% Uscita:
%   SOC_out - SOC stimato [n_cells x 1]
    
    M=10; %finestra di misura per AEKF
    % Numero di celle 
    n_cells = 4;

    % Parametri batteria
    Ts = 0.1;           % Tempo campionamento [s]
    Q_cell = batt_param.c_cell;        % Capacità [Ah]
    R0_matrix = batt_param.r0;         % Resistenza ohmica [Ohm]
    R1_matrix = batt_param.r1;         % Resistenza RC [Ohm]
    C1_matrix = batt_param.c1;          % Capacità RC [F]
    
    SOC_grid =batt_param.soc(:);
    OCV_tab  =batt_param.ocv(:);

    % Pre-calcola derivata OCV
    n_points = length(SOC_grid);
    dSOC = SOC_grid(2) - SOC_grid(1);
    dOCV_temp = diff(OCV_tab) / dSOC;
    dOCV_tab = zeros(n_points, 1);
    dOCV_tab(1) = dOCV_temp(1);
    for i = 2:n_points-1
        dOCV_tab(i) = dOCV_temp(i);
    end
    dOCV_tab(n_points) = dOCV_temp(n_points-1);
     
    % % Rumore EKF
    % Q_cov = diag([1e-5, 1e-3]);
    % R_var = 0.00001;
    
   persistent SOC_state V1_state P_state H_k_vector Q_cov R_var initialized

if isempty(initialized) || reset
    SOC_state = 0.5 * ones(n_cells,1);
    V1_state  = zeros(n_cells,1);
    P_state   = zeros(2,2,n_cells);
    
    H_k_vector=    zeros(1,M);
    Q_cov = diag([1e-5, 1e-3]);
    R_var = 0.00001;
    
    for k = 1:n_cells
        P_state(:,:,k) = diag([0.05, 0.01]);
    end
    initialized = true;
end

%% PRE-ALLOCAZIONE

SOC_out = zeros(n_cells,1);
    H_k=0;
%% EKF PER OGNI CELLA

for k = 1:n_cells

    % Stato corrente
    x = [SOC_state(k); V1_state(k)];
    P = P_state(:,:,k);

    I_k = I_meas(k);
    V_k = V_meas(k);

    %% PREDIZIONE 
    % Coulomb counting
    SOC_pred = x(1) - (Ts * I_k) / (Q_cell(k) * 3600);
    SOC_pred = max(0, min(1, SOC_pred)); %limito il soc tra 0 e 1

    % ESTRAGGO LA RIGA k DALLA MATRICE E FACCIO L'INTERPOLAZIONE
    R0_row = R0_matrix(k, :);  % Riga k, vettore [1 x 21]
    R1_row = R1_matrix(k, :);  % Riga k, vettore [1 x 21]
    C1_row = C1_matrix(k, :);  % Riga k, vettore [1 x 21]
        
    % Converte a colonna per interp1
    R0_col = R0_row(:);  % [21 x 1]
    R1_col = R1_row(:);  % [21 x 1]
    C1_col = C1_row(:);  % [21 x 1]

    % Interpolazione
    R0 = interp1(SOC_grid, R0_col, SOC_pred, 'linear', 'extrap');
    R1 = interp1(SOC_grid, R1_col, SOC_pred, 'linear', 'extrap');
    C1 = interp1(SOC_grid, C1_col, SOC_pred, 'linear', 'extrap');

    tau1  = R1 * C1;
    alpha = exp(-Ts / tau1);

    % Dinamica RC
    V1_pred = alpha * x(2) + R1 * (1 - alpha) * I_k;

    x_pred = [SOC_pred; V1_pred];

    % Jacobiano stato
    A = [1, 0;
         0, alpha];

    P_pred = A * P * A' + Q_cov;

    %% AGGIORNAMENTO
    OCV  = interp1(SOC_grid, OCV_tab, SOC_pred,"linear","extrap");
    dOCV = interp1(SOC_grid, dOCV_tab, SOC_pred,"linear","extrap");
        
    V_pred = OCV - x_pred(2) - R0 * I_k;
    y_tilde = V_k - V_pred;

    C_mat = [dOCV, -1];

    S = C_mat * P_pred * C_mat' + R_var;
    K = (P_pred * C_mat') / S;

    % Aggiornamento stato
    x_upd = x_pred + K * y_tilde;
    x_upd(1) = max(0, min(1, x_upd(1)));

    % Aggiornamento covarianza
    I2 = eye(2);
    P_upd = (I2 - K * C_mat) * P_pred;

    H_k_vector=circshift(H_k_vector,1);
    H_k_vector(1)=y_tilde*y_tilde';
    if H_k_vector(M)~=0
        H_k=sum(H_k_vector)/M;
        Q_cov=K*H_k*K';
        R_var=H_k-C_mat*P_upd*C_mat';
    end
    %% SALVATAGGIO 
    SOC_state(k) = x_upd(1);
    V1_state(k)  = x_upd(2);
    P_state(:,:,k) = P_upd;



    SOC_out(k) = x_upd(1);
        
end
end