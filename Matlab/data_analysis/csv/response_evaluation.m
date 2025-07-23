% Dynamic Response Evaluation (CSV)

% Filter data within the specified time range
Tmax = 20;

idx_x = t_x <= Tmax;
idx_y = t_y <= Tmax;
idx_z = t_z <= Tmax;
idx_rot = t_rot <= Tmax;

% Import CSV data
data_x = readmatrix('trans_x.csv');
data_y = readmatrix('trans_y.csv');
data_z = readmatrix('trans_z.csv');
data_rot = readmatrix('rot_z.csv');

t_x = data_x(:,1);  x = data_x(:,2);
t_y = data_y(:,1);  y = data_y(:,2);
t_z = data_z(:,1);  z = data_z(:,2);
t_rot = data_rot(:,1);  rot_z = data_rot(:,2);

t_x = t_x(idx_x);  x = x(idx_x);
t_y = t_y(idx_y);  y = y(idx_y);
t_z = t_z(idx_z);  z = z(idx_z);
t_rot = t_rot(idx_rot);  rot_z = rot_z(idx_rot);

% Target values
target_x = -800;
target_y = -800;
target_z =  800;
target_rot = 0;

% Common display function
printStepInfo = @(label, info, unit) fprintf([...
    '\n%s response info:\n' ...
    '  Rise Time          : %.2f s\n' ...
    '  Settling Time      : %.2f s\n' ...
    '  Overshoot          : %.2f %%\n' ...
    '  Undershoot         : %.2f %%\n' ...
    '  Peak Time          : %.2f s\n' ...
    '  Peak               : %.2f %s\n' ...
    '  Steady-State Error : %.2f %s\n'], ...
    label, info.RiseTime, info.SettlingTime, ...
    info.Overshoot, info.Undershoot, info.PeakTime, ...
    info.Peak, unit, info.SteadyStateError, unit);

% ------- X-axis -------
ts = linspace(t_x(1), t_x(end), length(t_x));
ys = interp1(t_x, x, ts, 'linear');
normed = (ys - ys(1)) / (target_x - ys(1));
info_x = stepinfo(normed, ts);
info_x.SteadyStateError = abs(ys(end) - target_x);
info_x.Peak = max(ys);  % Use actual peak value
printStepInfo('X-axis', info_x, 'mm');

% ------- Y-axis -------
ts = linspace(t_y(1), t_y(end), length(t_y));
ys = interp1(t_y, y, ts, 'linear');
normed = (ys - ys(1)) / (target_y - ys(1));
info_y = stepinfo(normed, ts);
info_y.SteadyStateError = abs(ys(end) - target_y);
info_y.Peak = max(ys);
printStepInfo('Y-axis', info_y, 'mm');

% ------- Z-axis -------
ts = linspace(t_z(1), t_z(end), length(t_z));
ys = interp1(t_z, z, ts, 'linear');
normed = (ys - ys(1)) / (target_z - ys(1));
info_z = stepinfo(normed, ts);
info_z.SteadyStateError = abs(ys(end) - target_z);
info_z.Peak = max(ys);
printStepInfo('Z-axis', info_z, 'mm');

% ------- Rotation Z -------
ts = linspace(t_rot(1), t_rot(end), length(t_rot));
ys = interp1(t_rot, rot_z, ts, 'linear');
normed = (ys - ys(1)) / (target_rot - ys(1));
info_rot = stepinfo(normed, ts);
info_rot.SteadyStateError = abs(ys(end) - target_rot);
info_rot.Peak = max(ys);
printStepInfo('Rot Z', info_rot, 'deg');

% Additional overshoot/undershoot analysis (based on raw data)
max_val = max(rot_z);
min_val = min(rot_z);

overshoot = max(0, max_val - target_rot);
undershoot = max(0, target_rot - min_val);

fprintf('\n[Raw Data Analysis]\n');
fprintf('Rot Z Overshoot  = %.2f deg\n', overshoot);
fprintf('Rot Z Undershoot = %.2f deg\n', undershoot);
