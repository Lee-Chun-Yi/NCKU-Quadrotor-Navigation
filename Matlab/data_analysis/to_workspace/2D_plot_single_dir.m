% Single-direction 2D-plot
% ==== User Input ====
mode = 'pitch';   % Options: 'roll', 'pitch', 'throttle'
t_max = 20;
y_min = -1500;
y_max = 1300;
X_displacement = 9.82
Y_displacement = -1118.49

% ==== Mode Selection ====
switch lower(mode)
    case 'roll'
        signal = out.trans_y;
        label = 'Roll (trans\_y)';
    case 'pitch'
        signal = out.trans_x;
        label = 'Pitch (trans\_x)';
    case 'throttle'
        signal = out.trans_z;
        label = 'Throttle (trans\_z)';
    otherwise
        error('Unknown mode: %s. Please choose "roll", "pitch", or "throttle".', mode);
end

% ==== Data Processing ====
t = signal.Time;
y = squeeze(signal.Data);
if isrow(y), y = y'; end

% ==== Plotting ====
figure;
plot(t-X_displacement, y-Y_displacement, 'b', 'LineWidth', 1.5);
grid on;
axis([0 t_max 0 y_max]);

xlabel('Time (s)');
ylabel(label, 'Interpreter', 'none');
title(sprintf('%s vs Time', label), 'Interpreter', 'none');
