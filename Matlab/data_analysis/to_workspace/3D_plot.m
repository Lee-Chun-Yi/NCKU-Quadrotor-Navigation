% 3D Flight Trajectory Plot - To workspace

% Read CSV files (assuming units in mm, convert to meters)
data_x = readmatrix('trans_x_1.csv');
data_y = readmatrix('trans_y_1.csv');
data_z = readmatrix('trans_z_1.csv');

% Extract time and position data
t_all = data_x(:,1);
x_all = data_x(:,2) / 1000;  % mm to m
y_all = data_y(:,2) / 1000;
z_all = data_z(:,2) / 1000;

% Limit time range (0 to 100 seconds)
t_range = (t_all >= 0) & (t_all <= 100);
t = t_all(t_range);
x = x_all(t_range);
y = y_all(t_range);
z = z_all(t_range);

% Compute velocity vectors and magnitude
vx = [0; diff(x)] ./ [1; diff(t)];
vy = [0; diff(y)] ./ [1; diff(t)];
vz = [0; diff(z)] ./ [1; diff(t)];
v_mag = sqrt(vx.^2 + vy.^2 + vz.^2);

% Plot 3D trajectory
figure;
scatter3(x, y, z, 24, v_mag, 'filled'); hold on;
plot3(x, y, z, 'b-', 'LineWidth', 1.2);
plot3(x(1), y(1), z(1), 'go', 'MarkerSize', 8, 'LineWidth', 1.5);      % Start point
plot3(x(end), y(end), z(end), 'ro', 'MarkerSize', 8, 'LineWidth', 1.5); % End point

% Add velocity direction arrows
N = 10; idx = 1:N:length(x);
quiver3(x(idx), y(idx), z(idx), vx(idx), vy(idx), vz(idx), 0.5, 'r');

% Styling
xlabel('X Position (m)');
ylabel('Y Position (m)');
zlabel('Z Position (m)');
title('3D Flight Trajectory with Velocity');
colorbar; colormap jet;
c = colorbar;
c.Label.String = 'Speed (m/s)';
grid on; axis equal;
view(135, 30);
legend('Speed Magnitude', 'Trajectory', 'Start', 'End');
