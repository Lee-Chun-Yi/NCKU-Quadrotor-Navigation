% 2D Plot - From Workspace Signals
% ==== User Input ====
Tmax = 80;  % Maximum time (in seconds)

% ==== Data Processing ====
% Retrieve Time and Data, and flatten to vectors
t_x   = out.trans_x.Time;
x     = squeeze(out.trans_x.Data);
t_y   = out.trans_y.Time;
y     = squeeze(out.trans_y.Data);
t_z   = out.trans_z.Time;
z     = squeeze(out.trans_z.Data);
t_rot = out.rot_z.Time;
rot_z = squeeze(out.rot_z.Data);

% Ensure all data vectors are column vectors
if isrow(x),     x = x';     end
if isrow(y),     y = y';     end
if isrow(z),     z = z';     end
if isrow(rot_z), rot_z = rot_z'; end

% Build logical indices for time filtering
idx_x   = t_x   <= Tmax;
idx_y   = t_y   <= Tmax;
idx_z   = t_z   <= Tmax;
idx_rot = t_rot <= Tmax;

% ==== Plotting ====
figure;

subplot(4,1,1);
plot(t_x(idx_x), x(idx_x), 'b', 'LineWidth', 1.2);
ylabel('X (mm)');
title('X vs Time');
grid on;

subplot(4,1,2);
plot(t_y(idx_y), y(idx_y), 'b', 'LineWidth', 1.2);
ylabel('Y (mm)');
title('Y vs Time');
grid on;

subplot(4,1,3);
plot(t_z(idx_z), z(idx_z), 'b', 'LineWidth', 1.2);
ylabel('Z (mm)');
title('Z vs Time');
grid on;

subplot(4,1,4);
plot(t_rot(idx_rot), rot_z(idx_rot), 'b', 'LineWidth', 1.2);
xlabel('Time (s)');
ylabel('Rot Z (deg)');
title('Rotation Z vs Time');
grid on;
