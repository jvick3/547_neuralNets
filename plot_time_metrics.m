function plot_time_metrics(input)

    display('Using metrics data file:')
    display(input)  

    % File exists?
    if (not (isequal(exist(input,'file'),2))) % 2 means it's a file. 
        error('ERROR: Metrics data file %input does not exist! ')
    end

    display('Metrics data file exists, proceeding...')
    
    metrics = dlmread(input);   % dlmread is "de-limiter read"
    
    % Data file has exactly two columns?
    if (size(metrics, 2) ~= 2)
        error('Metrics data file must have exactly two columns')
    end
    
    % Data file has been sanity-checked, do plotting
    % Each row of the file contain these columns:
    % n e
    % where n = discrete time step #, e = is energy of agent at time n
    n = metrics(:, 1);
    energy = metrics(:, 2);
    
    plot(n, energy, 'g')  % plot n versus energy in green
    xlabel('Time step n')
    ylabel('Energy of Agent')
    
    
end