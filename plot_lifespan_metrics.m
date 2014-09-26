function plot_lifespan_metrics(input)

    display('Using metrics data file:')
    display(input)  

    % File exists?
    if (not (isequal(exist(input,'file'),2))) % 2 means it's a file. 
        error('ERROR: Metrics data file %input does not exist! ')
    end

    display('Metrics data file exists, proceeding...')
    
    metrics = dlmread(input);   % dlmread is "de-limiter read"
    
    % Data file has exactly six columns?
    if (size(metrics, 2) ~= 6)
        error('Metrics data file must have exactly six columns')
    end
    
    % Data file has been sanity-checked, do plotting
    % Each row of the file contain these columns:
    % number_of_neurons lifespan total_food_eaten beneficial_food_eaten neutral_food_eaten harmful_food_eaten
    neurons = metrics(:, 1);
    lifespan = metrics(:, 2);
    total_food_eaten = metrics(:, 3);
    beneficial_food_eaten = metrics(:, 4);
    neutral_food_eaten = metrics(:, 5);
    harmful_food_eaten = metrics(:, 6);
    
    %TODO: add more plots to be made...
    plot(total_food_eaten, lifespan)
    xlabel('Food items eaten')
    ylabel('Agent lifespan (steps)')
    
    
end