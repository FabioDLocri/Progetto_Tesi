function [SOC_est, Cap_est, Uocv_est, R0_est, Rp_est, Cp_est] = ...
         CoEstimation_4Cells(U_cell, I_cell, T_cell, Ts)
    
    % PARAMETRI PERSISTENTI per tutte e 4 le celle
    persistent init_flag
    persistent FFRLS_params IAEKF_params OCV_params
    
    % Inizializzazione
    if isempty(init_flag)
        init_flag = 1;
        [FFRLS_params, IAEKF_params, OCV_params] = initialize_parameters();
    end
    
    % Preallocazione output
    SOC_est = zeros(4,1);
    Cap_est = zeros(4,1);
    Uocv_est = zeros(4,1);
    R0_est = zeros(4,1);
    Rp_est = zeros(4,1);
    Cp_est = zeros(4,1);
    
    % Loop su tutte e 4 le celle
    for cell_idx = 1:4
        % Estrai dati cella corrente
        Ui = U_cell(cell_idx);
        I = I_cell(cell_idx);
        T = T_cell(cell_idx); % Se non disponibile, usare valore fisso
        
        % 1. FFRLS per identificazione parametri e OCV
        [R0, Rp, Cp, Uocv] = FFRLS_cell(Ui, I, Ts, ...
            FFRLS_params{cell_idx}.Phi, ...
            FFRLS_params{cell_idx}.theta, ...
            FFRLS_params{cell_idx}.P, ...
            FFRLS_params{cell_idx}.lambda);
        
        % Aggiorna parametri FFRLS persistenti
        FFRLS_params{cell_idx}.Phi = [Ui, I]; % Aggiorna vettore misura
        FFRLS_params{cell_idx}.theta = [R0, Rp, Cp];
        FFRLS_params{cell_idx}.P = eye(3); % Semplificato
        
        % 2. IAEKF per co-stima SOC e Capacità
        [SOC, Cap] = IAEKF_cell(Uocv, I, Ts, T, ...
            IAEKF_params{cell_idx}.x, ...
            IAEKF_params{cell_idx}.P, ...
            IAEKF_params{cell_idx}.Q, ...
            IAEKF_params{cell_idx}.R, ...
            IAEKF_params{cell_idx}.lambda_IAEKF);
        
        % Aggiorna stato IAEKF
        IAEKF_params{cell_idx}.x = [SOC; Cap];
        
        % 3. Calcola OCV stimata dal modello polinomiale (verifica)
        Uocv_poly = OCV_model(SOC, T, OCV_params.k);
        
        % Output
        SOC_est(cell_idx) = SOC;
        Cap_est(cell_idx) = Cap;
        Uocv_est(cell_idx) = Uocv_poly;
        R0_est(cell_idx) = R0;
        Rp_est(cell_idx) = Rp;
        Cp_est(cell_idx) = Cp;
    end
end

function [FFRLS_params, IAEKF_params, OCV_params] = initialize_parameters()
    % Inizializza parametri per 4 celle
    
    FFRLS_params = cell(4,1);
    IAEKF_params = cell(4,1);
    
    for i = 1:4
        % Parametri FFRLS
        FFRLS_params{i}.Phi = [0, 0]; % [U_prev, I_prev]
        FFRLS_params{i}.theta = [0.03, 0.02, 2000]; % [R0, Rp, Cp] iniziali
        FFRLS_params{i}.P = 1e3 * eye(3);
        FFRLS_params{i}.lambda = 0.98; % Forgetting factor
        
        % Parametri IAEKF
        IAEKF_params{i}.x = [0.5; 1.0]; % [SOC_0; Cap_0] - Cap_0 = 1Ah
        IAEKF_params{i}.P = eye(2);
        IAEKF_params{i}.Q = diag([1e-3, 0]); % Rumore di processo
        IAEKF_params{i}.R = 1e1; % Rumore di misura
        IAEKF_params{i}.lambda_IAEKF = 0.002; % Fattore di controllo per correzione
        IAEKF_params{i}.det_threshold = 0.045; % Soglia per correzione (V)
    end
    
    % Coefficienti polinomiali OCV-SOC-T (dalla Tabella 6)
    OCV_params.k = [2.749, 1.261e-6, -2.95e-8, 20.343, 0.033, -0.0011, ...
                    -236.4, -0.612, 0.023, 1513, 4.759, -0.201, ...
                    -5717, -18.797, 0.894, 13360, 40.925, -2.265, ...
                    -19530, -49.684, 3.398, 17670, 31.375, -2.981, ...
                    -8590, -7.885, 1.412, 1814, -0.116, -0.278];
end

function [R0, Rp, Cp, Uocv] = FFRLS_cell(Ui, I, Ts, prev_Phi, prev_theta, prev_P, lambda)
    % FFRLS per identificazione parametri modello RC
    
    % Misura corrente
    Phi_k = [prev_Phi(1), I, prev_Phi(2)]; % [U_{k-1}, I_k, I_{k-1}]
    
    % Vettore parametri atteso: theta = [a1, a2, a3, a4]
    % Ma qui identifichiamo direttamente R0, Rp, Cp
    
    % Calcolo guadagno di Kalman
    K = prev_P * Phi_k' / (Phi_k * prev_P * Phi_k' + 1/lambda);
    
    % Errore di predizione
    y_est = Phi_k * prev_theta;
    y_meas = Ui;
    e = y_meas - y_est;
    
    % Aggiornamento parametri
    theta_new = prev_theta + K * e;
    
    % Aggiornamento matrice di covarianza
    P_new = (eye(3) - K * Phi_k) * prev_P / lambda;
    
    % Estrai parametri fisici
    R0 = max(0.001, theta_new(1)); % Evita valori negativi
    Rp = max(0.001, theta_new(2));
    Cp = max(100, theta_new(3)); % Minimo 100F
    
    % Stima OCV (semplificata)
    % In pratica: Uocv = Ui + I*R0 + Up (dove Up si calcola dal modello RC)
    % Per semplicità:
    Uocv = Ui + I * R0;
    
    % Limita OCV a range ragionevole
    Uocv = min(max(Uocv, 2.5), 4.2);
end

function [SOC, Cap] = IAEKF_cell(Uocv_meas, I, Ts, T, x_prev, P_prev, Q, R, lambda_IAEKF)
    % IAEKF per co-stima SOC e Capacità
    
    % Stato precedente
    SOC_prev = x_prev(1);
    Cap_prev = x_prev(2);
    
    % ========== PREDIZIONE ==========
    % Modello di stato (Eq. 13)
    eta = 1; % Efficienza Coulombica
    SOC_pred = SOC_prev - eta * I * Ts / (3600 * Cap_prev);
    Cap_pred = Cap_prev; % Capacità varia lentamente
    
    % Jacobiana A (Eq. 16)
    A = [1, -eta * I * Ts / (3600 * Cap_prev^2);
         0, 1];
    
    % Predizione covarianza
    P_pred = A * P_prev * A' + Q;
    
    % ========== CORREZIONE ==========
    % Misura predetta (OCV dal modello polinomiale)
    Uocv_pred = OCV_model(SOC_pred, T, OCV_params.k);
    
    % Jacobiana C
    C = [dOCV_dSOC(SOC_pred, T, OCV_params.k), 0]; % Derivata rispetto a SOC
    
    % Guadagno di Kalman
    K = P_pred * C' / (C * P_pred * C' + R);
    
    % Innovazione
    d = Uocv_meas - Uocv_pred;
    
    % Correzione con fattore di controllo adattivo (miglioramento IAEKF)
    if abs(d) > 0.045 % det_threshold
        % Correzione normale
        x_corr = [SOC_pred; Cap_pred] + K * d;
    else
        % Correzione attenuata per stabilità
        x_corr = [SOC_pred; Cap_pred] + lambda_IAEKF * K * d;
    end
    
    % Limita SOC tra 0 e 1
    x_corr(1) = min(max(x_corr(1), 0), 1);
    
    % Limita capacità tra 0.8 e 1.2 Ah (20% variazione attesa)
    x_corr(2) = min(max(x_corr(2), 0.8), 1.2);
    
    % Aggiornamento covarianza
    P_corr = (eye(2) - K * C) * P_pred;
    
    % ========== OUTPUT ==========
    SOC = x_corr(1);
    Cap = x_corr(2);
end

function Uocv = OCV_model(SOC, T, k)
    % Modello polinomiale OCV-SOC-T (Eq. 17)
    % k: vettore di 30 coefficienti
    
    x = SOC;
    y = T;
    
    % Calcolo polinomio bidimensionale (9° grado in SOC, 2° in T)
    Uocv = k(1) + k(2)*y + k(3)*y^2 + ...
           (k(4) + k(5)*y + k(6)*y^2)*x + ...
           (k(7) + k(8)*y + k(9)*y^2)*x^2 + ...
           (k(10) + k(11)*y + k(12)*y^2)*x^3 + ...
           (k(13) + k(14)*y + k(15)*y^2)*x^4 + ...
           (k(16) + k(17)*y + k(18)*y^2)*x^5 + ...
           (k(19) + k(20)*y + k(21)*y^2)*x^6 + ...
           (k(22) + k(23)*y + k(24)*y^2)*x^7 + ...
           (k(25) + k(26)*y + k(27)*y^2)*x^8 + ...
           (k(28) + k(29)*y + k(30)*y^2)*x^9;
end

function dU_dSOC = dOCV_dSOC(SOC, T, k)
    % Derivata dell'OCV rispetto a SOC
    x = SOC;
    y = T;
    
    dU_dSOC = (k(4) + k(5)*y + k(6)*y^2) + ...
              2*(k(7) + k(8)*y + k(9)*y^2)*x + ...
              3*(k(10) + k(11)*y + k(12)*y^2)*x^2 + ...
              4*(k(13) + k(14)*y + k(15)*y^2)*x^3 + ...
              5*(k(16) + k(17)*y + k(18)*y^2)*x^4 + ...
              6*(k(19) + k(20)*y + k(21)*y^2)*x^5 + ...
              7*(k(22) + k(23)*y + k(24)*y^2)*x^6 + ...
              8*(k(25) + k(26)*y + k(27)*y^2)*x^7 + ...
              9*(k(28) + k(29)*y + k(30)*y^2)*x^8;
end