function [SOC, C] = soc_capacity_4cells(V, I, batt_param)

Ts = 0.1;
Nc = 4;
gamma = 0.98;

% ===== PERSISTENT =====
persistent init
persistent theta P_theta
persistent x P Q R
persistent V_prev I_prev


if isempty(init)
    init = true;

    theta   = zeros(4, Nc);
    P_theta = zeros(4,4,Nc);

    x = zeros(2, Nc);
    P = zeros(2,2,Nc);

    Q = repmat(diag([1e-6 1e-8]), [1 1 Nc]);
    R = repmat(1e-3, [1 1 Nc]);

    V_prev = V;
    I_prev = I;

    for c = 1:Nc

        % ===== STATI =====
        SOC0 = 0.8;       % SOC iniziale
        C0   = batt_param.c_cell(c);        % Capacità iniziale [Ah]

        x(:,c) = [SOC0; C0];
        P(:,:,c) = diag([0.01 1]);

        % ===== PARAMETRI MODELLO =====
        R0_0 = soc_map_lookup(SOC0, batt_param.r0(c,:));    % Ohm
        Rp_0 = soc_map_lookup(SOC0, batt_param.r1(c,:));     % Ohm
        Cp_0 = soc_map_lookup(SOC0, batt_param.c1(c,:));      % F
        OCV0 = soc_map_lookup(SOC0, batt_param.ocv);       % V
        
        
        alpha1 = exp(-Ts/(Rp_0*Cp_0));
        alpha3 = -R0_0;
        alpha4 = alpha1*R0_0 - (1-alpha1)*Rp_0;
        alpha2 = (1-alpha1)*OCV0;

        theta(:,c) = [alpha1; alpha2; alpha3; alpha4];
        P_theta(:,:,c) = eye(4)*1e3;
    end
end

SOC = zeros(Nc,1);
C   = zeros(Nc,1);

% ===== LOOP CELLE =====
for c = 1:Nc

    %% ===== FFRLS =====
    phi = [V_prev(c), 1, I(c), I_prev(c)];
    y   = V(c);

    Pth = P_theta(:,:,c);
    th  = theta(:,c);

    K = Pth*phi'/(phi'*Pth*phi + gamma);
    err = y - phi*th;

    th = th + K*err;
    Pth = (eye(4) - K*phi)*Pth/gamma;

    theta(:,c) = th;
    P_theta(:,:,c) = Pth;

    alpha1 = th(1);
    alpha2 = th(2);
    Uocv_hat = alpha2/(1 - alpha1);

%% ===============================================================================
%% ===============================================================================
    %% ===== IAEKF =====
    SOCk = x(1,c);
    Ck   = x(2,c);

    SOCp = SOCk - Ts*I_prev(c)/(3600*Ck);
    Cp   = Ck;

    xp = [SOCp; Cp];

    A = [1 Ts*I_prev(c)/(3600*Ck^2); 0 1];
    Pp = A*P(:,:,c)*A' + Q(:,:,c);

    % OCV da curva
    Uocv_pred = soc_map_lookup(SOCp, batt_param.ocv);
    dU_dSOC   = docv_dsoc_lookup(SOCp, batt_param.ocv);

    H = [dU_dSOC 0];

    Kk = Pp*H'/(H*Pp*H' + R(:,:,c));

    d = Uocv_hat - Uocv_pred;

    if abs(d) > 0.045
        lambda = 1;
    else
        lambda = 0.002;
    end

    x(:,c) = xp + lambda*Kk*d;
    P(:,:,c) = (eye(2) - Kk*H)*Pp;

    SOC(c) = x(1,c);
    C(c)   = x(2,c);
end

V_prev = V;
I_prev = I;
end

%% Funzioni Macro per calcolare il valore dalla Lookup table e la derivata del SOC

function R0 = soc_map_lookup(SOC, map_row)
% SOC ∈ [0,1]
% map_row ∈ [1x21]

N = 21;
step = 1/(N-1);

idx = floor(SOC/step) + 1;

if idx < 1
    idx = 1;
elseif idx >= N
    idx = N-1;
end

SOC_l = (idx-1)*step;
SOC_h = idx*step;

w = (SOC - SOC_l)/(SOC_h - SOC_l);

R0 = map_row(idx)*(1-w) + map_row(idx+1)*w;
end

function dU = docv_dsoc_lookup(SOC, OCV_map)

N = 21;
step = 1/(N-1);

idx = floor(SOC/step) + 1;

if idx < 2
    idx = 2;
elseif idx > N-1
    idx = N-1;
end

dU = (OCV_map(idx+1) - OCV_map(idx-1)) / (2*step);
end
