vars = {'rot_z', 'trans_x', 'trans_y', 'trans_z'};
idx = 1;

for i = 1:length(vars)
    varName = vars{i};
    ts = out.(varName);  % Retrieve timeseries object

    if isa(ts, 'timeseries')
        t = ts.Time;
        d = squeeze(ts.Data);  % Flatten 3D → 1D vector

        % Ensure it's a column vector
        if isrow(d)
            d = d';
        end

        % Check for matching length between Time and Data
        if numel(t) ~= numel(d)
            warning('"%s" time and data length mismatch: Time = %d, Data = %d. Skipped.', ...
                varName, numel(t), numel(d));
            continue;
        end

        % Create table
        T = table(t, d, 'VariableNames', {'Time', varName});

        % Create filename with fixed index
        filename = sprintf('%s_%d.csv', varName, idx);

        % Write to CSV file
        writetable(T, filename);
        fprintf('Saved %s (%d rows)\n', filename, height(T));
    else
        fprintf('"%s" is not a timeseries object, skipped.\n', varName);
    end
end
