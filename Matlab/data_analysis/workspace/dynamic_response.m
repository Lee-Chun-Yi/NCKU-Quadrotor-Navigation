% Dynamic Response Evaluation
% ==== User Input ====
Tmax = 20;  % Time window (s)

% Target values
target_x   = -800;
target_y   = -800;
target_z   =  800;
target_rot = 0;

% ==== Data Processing ====

% Extract internal times and signals
t_x   = out.trans_x.Time;   x     = out.trans_x.Data;
t_y   = out.trans_y.Time;   y     = out.trans_y.Data;
t_z   = out.trans_z.Time;   z     = out.trans_z.Data;
t_rot = out.rot_z.Time;     rot_z = out.rot_z.Data;

% Filter data within time limits
idx_x   = t_x   <= Tmax;
idx_y   = t_y   <= Tmax;
idx_z   = t_z   <= Tmax;
idx_rot = t_rot <= Tmax;

t_x   = t_x(idx_x);   x     = x(idx_x);
t_y   = t_y(idx_y);   y     = y(idx_y);
t_z   = t_z(idx_z);   z     = z(idx_z);
t_rot = t_rot(idx_rot); rot_z = rot_z(idx_rot);

% ==== Response Evaluation Function ====

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

% ==== Step Response Computation ====

% ------- X Axis -------
ts = linspace(t_x(1), t_x(end), length(t_x));
ys = interp1(t_x, x, ts, 'linear');
normed = (ys - ys(1)) / (target_x - ys(1));
info_x = stepinfo(normed, ts);
info_x.SteadyStateError = abs(ys(end) - target_x);
info_x.Peak = max(ys);
printStepInfo('X-axis', info_x, 'mm');

% ------- Y Axis -------
ts = linspace(t_y(1), t_y(end), length(t_y));
ys = interp1(t_y, y, ts, 'linear');
normed = (ys - ys(1)) / (target_y - ys(1));
info_y = stepinfo(normed, ts);
info_y.SteadyStateError = abs(ys(end) - target_y);
info_y.Peak = max(ys);
printStepInfo('Y-axis', info_y, 'mm');

% ------- Z Axis -------
ts = linspace(t_z(1), t_z(end), length(t_z));
ys = interp1(t_z, z, ts, 'linear');
normed = (ys - ys(1)) / (target_z - ys(1));
info_z = stepinfo(normed, ts);
info_z.SteadyStateError = abs(ys(end) - target_z);
info_z.Peak = max(ys);
printStepInfo('Z-axis', info_z, 'mm');

% ------- Rot Z -------
ts = linspace(t_rot(1), t_rot(end), length(t_rot));
ys = interp1(t_rot, rot_z, ts, 'linear');
normed = (ys - ys(1)) / (target_rot - ys(1));
info_rot = stepinfo(normed, ts);
info_rot.SteadyStateError = abs(ys(end) - target_rot);
info_rot.Peak = max(ys);
printStepInfo('Rot Z', info_rot, 'deg');

% ==== Raw Overshoot/Undershoot (non-normalized) ====
max_val = max(rot_z);
min_val = min(rot_z);

overshoot   = max(0, max_val - target_rot);
undershoot  = max(0, target_rot - min_val);

fprintf('\n[Raw Data Analysis]\n');
fprintf('Rot Z Overshoot  = %.2f deg\n', overshoot);
fprintf('Rot Z Undershoot = %.2f deg\n', undershoot);
