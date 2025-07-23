% 3D Flight Trajectory Plot - From CSV
% ==== User Input ====

% Time range
t_min = 0;
t_max = 100;

% CSV file names (assumed in millimeters)
file_x = 'trans_x_1.csv';
file_y = 'trans_y_1.csv';
file_z = 'trans_z_1.csv';

% ==== Data Processing ====

% Read CSV files
data_x = readmatrix(file_x);
data_y = readmatrix(file_y);
data_z = readmatrix(file_z);

% Extract time and convert position to meters
t_all = data_x(:,1);
x_all = data_x(:,2) / 1000;  % mm to m
y_all = data_y(:,2) / 1000;
z_all = data_z(:,2) / 1000;

% Apply time filtering
t_range = (t_all >= t_min) & (t_all <= t_max);
t = t_all(t_range);
x = x_all(t_range);
y = y_all(t_range);
z = z_all(t_range);

% Compute velocity vectors and speed magnitude
vx = [0; diff(x)] ./ [1; diff(t)];
vy = [0; diff(y)] ./ [1; diff(t)];
vz = [0; diff(z)] ./ [1; diff(t)];
v_mag = sqrt(vx.^2 + vy.^2 + vz.^2);

% ==== Plotting ====

figure;
scatter3(x, y, z, 24, v_mag, 'filled'); hold on;
plot3(x, y, z, 'b-', 'LineWidth', 1.2);

% Mark start and end points
plot3(x(1), y(1), z(1), 'go', 'MarkerSize', 8, 'LineWidth', 1.5);
plot3(x(end), y(end), z(end), 'ro', 'MarkerSize', 8, 'LineWidth', 1.5);

% Add velocity direction arrows
N = 10;
idx = 1:N:length(x);
quiver3(x(idx), y(idx), z(idx), vx(idx), vy(idx), vz(idx), 0.5, 'r');

% Style and labels
xlabel('X Position (m)');
ylabel('Y Position (m)');
zlabel('Z Position (m)');
title('3D Flight Trajectory with Velocity');
grid on; axis equal;
view(135, 30);

% Color bar
colormap jet;
c = colorbar;
c.Label.String = 'Speed (m/s)';

% Legend
legend('Speed Magnitude', 'Trajectory', 'Start', 'End');
