% 3D PLOT (CSV)
% ==== User Input ====
idx = 1;
T_max = 15;

% ==== Filter Data within Time Range (without shifting coordinates) ====
time_range = (t_x >= 0) & (t_x <= T_max);

t = t_x(time_range);
x = x_all(time_range);
y = y_all(time_range);
z = z_all(time_range);

% Import CSV Data
data_x = readmatrix(sprintf('trans_x.csv_%d', idx));
data_y = readmatrix(sprintf('trans_y.csv_%d', idx));
data_z = readmatrix(sprintf('trans_z.csv_%d', idx));

% Extract Time and Position Data
t_x = data_x(:,1);  x_all = data_x(:,2);
t_y = data_y(:,1);  y_all = data_y(:,2);
t_z = data_z(:,1);  z_all = data_z(:,2);

% Calculate Velocities and Speed Magnitude
vx = [0; diff(x)];
vy = [0; diff(y)];
vz = [0; diff(z)];
v_mag = sqrt(vx.^2 + vy.^2 + vz.^2);

% Plot 3D Flight Trajectory
figure;
scatter3(x, y, z, 20, v_mag, 'filled'); hold on;
plot3(x, y, z, 'b-', 'LineWidth', 1);

% Mark Start and End Points
plot3(x(1), y(1), z(1), 'go', 'MarkerSize', 10);   % Start point
plot3(x(end), y(end), z(end), 'ro', 'MarkerSize', 10);  % End point

% Add Velocity Arrows
N = 10;
idx = 1:N:length(x);
quiver3(x(idx), y(idx), z(idx), vx(idx), vy(idx), vz(idx), 0.5, 'r');

% Plot Formatting
xlabel('X Position (m)');
ylabel('Y Position (m)');
zlabel('Z Position (m)');
title('3D Flight Trajectory (True Global Coordinates)');
colorbar; colormap jet;
grid on; axis equal;
view(135, 30);
legend('Speed', 'Trajectory', 'Start', 'End');
