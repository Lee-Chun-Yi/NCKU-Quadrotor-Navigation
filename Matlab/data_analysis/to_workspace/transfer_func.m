% Identify Transfer Function from Step Input Response
% ==== User Input ====
Step_input_magnitude = 0.2;       % Magnitude of step input
X_displacement = 9.82;            % Time shift (s)
Y_displacement = -1118.49;        % Signal bias correction (e.g., hover level)

% ==== Data Processing ====
% Extract and normalize response signal
y_all = out.trans_y.Data - Y_displacement;
t_all = out.trans_y.Time - X_displacement;

% Create step input signal
u_all = zeros(size(t_all));
u_all(t_all >= 0) = Step_input_magnitude;

% Filter data between 0 and 15 seconds
idx = (t_all >= 0) & (t_all <= 15);
t = t_all(idx);
y = y_all(idx);
u = u_all(idx);

% Compute sampling time
Ts = mean(diff(t));

% ==== System Identification ====
% Build identification dataset
data_id = iddata(y, u, Ts);

% Estimate 2nd-order transfer function with 1 input delay
sys = tfest(data_id, 2, 1);

% ==== Result Visualization ====
compare(data_id, sys);   % Compare response
step(sys);               % Step response of identified model
disp(sys);               % Display transfer function

% Extract numerator and denominator
[num, den] = tfdata(sys, 'v');
disp('Estimated transfer function:');
G = tf(num, den);        % Assign final TF to G
