% Multi-Flight Trajectory Visualization
% ==== User Input ====
idx_list = [1, 2];         % List of flight data indices to be loaded
Tmax = 10;                 % Maximum time (s)
t_ref = linspace(0, Tmax, 500);  % Unified time axis

% ==== Data Processing ====
% Initialize storage matrices
x_all = []; y_all = []; z_all = []; rot_all = [];

for i = 1:length(idx_list)
    idx = idx_list(i);

    % Load CSV data
    data_x   = readmatrix(sprintf('trans_x_%d.csv', idx));
    data_y   = readmatrix(sprintf('trans_y_%d.csv', idx));
    data_z   = readmatrix(sprintf('trans_z_%d.csv', idx));
    data_rot = readmatrix(sprintf('rot_z_%d.csv', idx));

    % Extract time and signal
    t_x   = data_x(:,1);   x     = data_x(:,2);
    t_y   = data_y(:,1);   y     = data_y(:,2);
    t_z   = data_z(:,1);   z     = data_z(:,2);
    t_rot = data_rot(:,1); rot_z = data_rot(:,2);

    % Interpolate onto common reference time axis
    x_all   = [x_all;   interp1(t_x,   x,     t_ref, 'linear', NaN)];
    y_all   = [y_all;   interp1(t_y,   y,     t_ref, 'linear', NaN)];
    z_all   = [z_all;   interp1(t_z,   z,     t_ref, 'linear', NaN)];
    rot_all = [rot_all; interp1(t_rot, rot_z, t_ref, 'linear', NaN)];
end

% ==== Plotting ====
figure;

% -- Subplot 1: X axis with legend handles
subplot(4,1,1);
[h_flights, h_fill, h_mean] = plotTrajectoryWithStats(x_all, t_ref, 'X (mm)', 'X vs Time');

% -- Subplot 2~4: Y, Z, Rot Z (without returning legend handles)
subplot(4,1,2);
plotTrajectoryWithStats(y_all, t_ref, 'Y (mm)', 'Y vs Time');

subplot(4,1,3);
plotTrajectoryWithStats(z_all, t_ref, 'Z (mm)', 'Z vs Time');

subplot(4,1,4);
plotTrajectoryWithStats(rot_all, t_ref, 'Rot Z (deg)', 'Rotation Z vs Time');

% Unified legend (top-right)
N = size(h_flights, 2);
labels = arrayfun(@(i) sprintf('Flight %d', i), 1:N, 'UniformOutput', false);
legend([h_flights(1:N), h_fill, h_mean], ...
       [labels, {'±1σ band', 'Mean trajectory'}], ...
       'Position', [0.82 0.85 0.1 0.1], 'Box', 'off');

% ==== Function: Plot Trajectory and Return Handles ====
function [h_flights, h_fill, h_mean] = plotTrajectoryWithStats(data_all, t, y_label, title_text)
    hold on;

    % Remove columns with NaN values
    valid = all(~isnan(data_all), 1);
    t = t(valid);
    data_all = data_all(:, valid);

    % Plot each individual flight (thin, semi-transparent lines)
    cmap = lines(size(data_all,1));
    h_flights = gobjects(1, size(data_all,1));
    for i = 1:size(data_all,1)
        h_flights(i) = plot(t, data_all(i,:), '-', ...
            'Color', [cmap(i,:), 0.6], 'LineWidth', 0.8);
    end

    % Compute mean and standard deviation
    mu = mean(data_all, 1);
    sigma = std(data_all, 0, 1);

    % Plot ±1σ region as gray fill
    h_fill = fill([t, fliplr(t)], [mu+sigma, fliplr(mu-sigma)], ...
        [0.8 0.8 0.8], 'FaceAlpha', 0.3, 'EdgeColor', 'none');

    % Plot mean trajectory
    h_mean = plot(t, mu, '--', 'Color', [0 0 1 0.6], 'LineWidth', 2);

    ylabel(y_label); title(title_text);
    grid on;
    if contains(title_text, 'Rotation')
        xlabel('Time (s)');
    end
end
