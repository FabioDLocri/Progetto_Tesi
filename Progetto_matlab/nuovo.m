function [SOC_out,Cap_out]= Coestimation_4_cell (V_cell,I_cell,batt_param)

n_cells=4;
gamma = 0.98; 
Ts=0.1;
lambda=0.002;
alpha=0.98;

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


persistent init V_prec I_prec theta P_theta phi 
persistent x P Q R

if isempty(init)
    init=true;
    P_theta=zeros(4,4,n_cells);
    theta=zeros(4,n_cells);
    
    x= zeros(2,n_cells);
    P= zeros(2,2,n_cells);
    Q= zeros(2,2,n_cells);
    R= zeros(1,n_cells);


    for c=1:n_cells
        
        V_prec=V_cell;
        I_prec=I_cell;
        
        SOC0=1;
        C0 = batt_param.c_cell(c);

        

        % ESTRAGGO LA RIGA k DALLA MATRICE E FACCIO L'INTERPOLAZIONE
        R0_row = R0_matrix(c, :);  % Riga k, vettore [1 x 21]
        R1_row = R1_matrix(c, :);  % Riga k, vettore [1 x 21]
        C1_row = C1_matrix(c, :);  % Riga k, vettore [1 x 21]
        
        % Converte a colonna per interp1
        R0_col = R0_row(:);  % [21 x 1]
        R1_col = R1_row(:);  % [21 x 1]
        C1_col = C1_row(:);  % [21 x 1]

        % Interpolazione
        R0_0 = interp1(SOC_grid, R0_col, SOC0, 'linear', 'extrap');
        Rp_0 = interp1(SOC_grid, R1_col, SOC0, 'linear', 'extrap');
        Cp_0 = interp1(SOC_grid, C1_col, SOC0, 'linear', 'extrap');
        OCV0 = interp1(SOC_grid, OCV_tab, SOC0, 'linear', 'extrap');

        alpha1 = exp(-Ts/(Rp_0*Cp_0));
        alpha2 = (1-alpha1)*OCV0;
        alpha3 = -R0_0;
        alpha4 = alpha1 - (1-alpha1)*Rp_0;
        



        theta(:,c) = [alpha1; alpha2; alpha3; alpha4];
        P_theta(:,:,c) = eye(4)*1e3;
        
        x(:,c) = [SOC0;C0];
        P(:,:,c) = diag([1 1]);
        Q(:,:,c) = diag([1e-3 1e-3]);
        R(c)=1e1;

    end

end

SOC_out= zeros(n_cells,1);
Cap_out=zeros(n_cells,1);

for c=1:n_cells
    %% FFRLS
    y=V_cell(c);
    phi=[V_prec(c), 1, I_cell(c), I_prec(c)];
    
    Pth_prec = P_theta(:,:,c);
    th_prec = theta(:,c);
    
    %Time update

    K_theta = Pth_prec*phi'/(phi*Pth_prec*phi' + gamma);
    Pth=Pth_prec*(eye(4) - K_theta*phi)/gamma;

    %Measurement update
    err = y - phi*th_prec;
    th = th_prec + K_theta*err;

    theta(:,c) = th;
    P_theta(:,:,c) = Pth;
    
    Vocv=th(2)/(1-th(1));

    %% IAEKF
    
    SOCk=x(1,c);
    Ck= x(2,c);
    
    Pk=P(:,:,c);
    Qk=Q(:,:,c);
    Rk=R(c);

    SOC_prev=SOCk -(Ts*I_prec(c))/(3600*Ck);
    x(1,c)=SOC_prev;
    %secondo nino Xk va intero

    A = [1, (Ts*I_cell(c))/(3600*Ck^2);
        0,  1];
    
    OCV  = interp1(SOC_grid, OCV_tab, SOC_prev,"linear","extrap");
    dOCV= interp1(SOC_grid, dOCV_tab, SOC_prev, 'linear', 'extrap');
    
    C = [dOCV, (dOCV*Ts*I_cell(c))/(3600*Ck^2)];
    
    P_prev = A * Pk' * A + Qk; %potrebbe essere Ptrasposto, chissà

    K = P_prev * C'/(C*P_prev*C' + Rk);

    dk=Vocv - OCV;

    if dk>0.45
        x(:,c)=x(:,c) + K*dk;
    else
        x(:,c)=x(:,c) + lambda*K*dk;
    end
    
    Pk = (eye(2) - K*C)*P_prev;
    
    OCVmis=interp1(SOC_grid, OCV_tab, x(1,c),"linear","extrap");
    
    err=Vocv-OCVmis;

    Q(:,:,c)=Qk*alpha + (1-alpha)*(K*(dk*dk')*K');

    R(c)= alpha*Rk + (1-alpha)*(err*err' + C* P_prev* C');

    P(:,:,c)=Pk;

    SOC_out(c)=x(1,c);
    Cap_out(c)=x(2,c);
end
V_prec=V_cell;
I_prec=I_cell;
end