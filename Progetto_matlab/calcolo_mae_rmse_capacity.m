function [mae, rmse] = calcolo_mae_rmse_capacity(c, y_pred, t)

% INPUT:
%   y_true -> 4x1xN
%   c      -> [c1 c2 c3 c4] oppure 4x1
%   t      -> Nx1 (opzionale)
%
% OUTPUT:
%   mae  -> 1x4
%   rmse -> 1x4

    if ndims(y_pred) ~= 3 || size(y_pred,1) ~= 4
        error('Aspettati dati con dimensione 4x1xN.');
    end

    % Assicura c come 4x1
    c = c(:);
    if numel(c) ~= 4
        error('c deve avere 4 costanti.');
    end

    N = size(y_pred,3);

    % Crea y_pred costante: 4x1xN
    y_true = repmat(c, 1, 1, N);

    % Usa la funzione standard
     if ~isequal(size(y_pred), size(y_true))
        error('y_true e y_pred devono avere la stessa dimensione.');
    end

    if ndims(y_pred) ~= 3 || size(y_pred,1) ~= 4
        error('Aspettati dati con dimensione 4x1xN.');
    end

    % Converte da 4x1xN a Nx4
    y_true2 = squeeze(y_pred);   % diventa 4xN
    y_pred2 = squeeze(y_true);   % diventa 4xN
    y_true2 = y_true2.';         % Nx4
    y_pred2 = y_pred2.';         % Nx4

    err = y_true2 - y_pred2;

    % MAE/RMSE per canale
    mae = mean(abs(err), 1);
    rmse = sqrt(mean(err.^2, 1));

    % Stampa risultati
    fprintf('MAE  per Capacità:  '); fprintf('%.6f  ', mae); fprintf('\n');
    fprintf('RMSE per Capacità:  '); fprintf('%.6f  ', rmse); fprintf('\n');

    % Grafici (4 subplot)
    figure;
    for k = 1:4
        subplot(2,2,k);
        if nargin < 3 || isempty(t)
            plot(err(:,k), 'LineWidth', 1.2);
            xlabel('Campione');
        else
            t = t(:);
            if size(err,1) ~= numel(t)
                error('t deve avere la stessa lunghezza dei segnali (campioni).');
            end
            plot(t, err(:,k), 'LineWidth', 1.2);
            xlabel('Tempo [s]');
        end
        grid on;
        ylabel(sprintf('Errore Capacità Cella %d', k));
        title(sprintf('Cella %d: MAE=%.4f, RMSE=%.4f', k, mae(k), rmse(k)));
    end
end