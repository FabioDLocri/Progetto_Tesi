function SOH = calcolo_SOH(c, y_pred)

% Converte in 4xN
Q_meas = squeeze(y_pred);   % 4xN

% SOH istantaneo per cella (4xN)
SOH_vec = (Q_meas ./ c) * 100;

% SOH medio per cella (4x1)
SOH = mean(SOH_vec, 2);

for k=1:4
    
    fprintf('L SOH della Cella %d è %f \n',k,SOH(k));

end