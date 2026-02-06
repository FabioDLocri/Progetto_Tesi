function V_model = ecm_model_fun(p, t, I)
    % p = [R0, R1, C1, Voc]
    R0  = p(1);
    R1  = p(2);
    C1  = p(3);
    Voc = p(4);

    dt = diff(t);
    N  = numel(t);

    % Stato dinamico: tensione sul ramo RC
    Vrc = zeros(N,1);

    % Discretizzazione semplice di e^(-dt/(R1*C1))
    for k = 2:N
        alpha = exp(-dt(k-1)/(R1*C1));
        % Equazione discreta (assumendo I costante su [k-1,k])
        Vrc(k) = alpha*Vrc(k-1) + R1*(1-alpha)*I(k-1);
    end

    % Tensione ai terminali
    V_model = Voc - I.*R0 - Vrc;
end