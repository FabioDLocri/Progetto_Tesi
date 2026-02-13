function I_ts = generate_PCT_profile_f( ...
    I_pulse, ...      % ampiezza impulso [A] (scalare o vettore)
    t_pulse, ...      % durata impulso [s]
    t_relax, ...      % durata rilassamento [s]
    N_pulses, ...     % numero di impulsi
    Ts)               % sample time [s]

% Default sample time
if nargin < 5
    Ts = 1;
end

% Se I_pulse è scalare, lo replica (in questo modo posso passare come I_pulse
% un vettore con le varie ampiezze nel caso volessi impulsi di ampiezze
% diverse9
if isscalar(I_pulse)
    I_pulse = I_pulse * ones(N_pulses,1);
end

% Tempo totale
t_start = 100; % fase iniziale in cui la corrente è nulla, serve a HPPTCtest
t_single = floor(t_pulse + t_relax);
T_tot = N_pulses * t_single + t_start;

% Vettore tempo
t = (0:Ts:T_tot-Ts)';

% Preallocazione corrente
I = zeros(length(t),1);

% Costruzione impulsi
idx = t_start+1;% i primi t_start campioni la corrente è 0
for k = 1:N_pulses
    % Impulso
    idx_pulse = idx : idx + t_pulse/Ts - 1;
    I(idx_pulse) = I_pulse(k);

    % Avanza indice (impulso + relax)
    idx = idx + t_single/Ts;
end

% Timeseries per Simulink
I_ts = timeseries(I,t);

end
