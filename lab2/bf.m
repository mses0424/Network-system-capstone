
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% TA Section %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% DO NOT MODIFY THIS SECTION. Any changes will result in a penalty.
close all; clear; clc;
addpath ./ewa_function;
rng(77);

% Environment Configurations
freq = 24e9;
tx_node_number = 1;      % Number of Tx users
tx_antenna_number = 16;  % Number of Tx antennas
rx_node_number = 2;      % Number of Rx users
rx_antenna_number = 1;   % Number of Rx antennas
tx_beam_direction = 0:5:90; % degree
d = 0.5;                 % Distance between antennas (multiple of wavelength)
P_tx_dBm = 20;           % Transmission power of Tx (dBm)
N0_dBm = -88;            % Noise power (dBm)

filename = 'node_positions.txt'; 
fid = fopen(filename, 'r');
data = textscan(fid, '%s %f %f %f');
fclose(fid);
GS_coordinate = [data{2}(1), data{3}(1), data{4}(1)];
SAT_coordinate = [data{2}(2), data{3}(2), data{4}(2)];
tx_location = GS_coordinate; 
rx_location = SAT_coordinate; 
fprintf('Tx (GS) Coordinates: (%.2f, %.2f, %.2f)\n', tx_location);
fprintf('Rx (SAT) Coordinates: (%.2f, %.2f, %.2f)\n', rx_location);

resolution = 180;           % Number of angles dividing 180 degrees
%%%%%%%%%%%%%%%%%%%%%%%%%%%%% TA Section End %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

dx = rx_location(1) - tx_location(1);
dy = rx_location(2) - tx_location(2);
d_h = sqrt(dx^2 + dy^2);        
dz  = rx_location(3) - tx_location(3); 
theta_actual = abs(atand(d_h / dz));
fprintf('Rx Elevation Angle: %.6f degrees\n', theta_actual);
[~, idx] = min(abs(tx_beam_direction - theta_actual));
theta_optimal = tx_beam_direction(idx);
fprintf('Optimal beam: %.6f degrees\n\n', theta_optimal);

[a_steer, ~] = uniform(d, theta_optimal, tx_antenna_number);
wUser1 = a_steer;
angles = linspace(0.5, 90, resolution);
angles_rad = deg2rad(angles);   
wVec = -2*pi * d .* sin(angles_rad);
X1 = dtft(wUser1, wVec);          
tx_gain1 = abs(X1).^2;            

figure;
plot(angles, tx_gain1, 'r', 'LineWidth', 2);
xlabel('Angle'); ylabel('Beamforming Gain');
grid on; title('Tx Beamforming Gain vs. Angle for User1');

figure;
polarplot(angles_rad, tx_gain1, 'r', 'LineWidth', 2);
title('Polar Plot of Tx Beamforming Gain for User1');
grid on;

[~, idx_theta_actual] = min(abs(angles - theta_actual));
tx_gainuser1 = tx_gain1(180-idx_theta_actual);
rx_gain = 1;
dist1 = norm(rx_location - tx_location);
pluser1 = friis_equation(freq, tx_gainuser1, rx_gain , dist1);
noBF_tx_gain = 1;
pl_noBF = friis_equation(freq, noBF_tx_gain, rx_gain, dist1);
fprintf('Beamforming pathloss = %.6f dB\n', pluser1);
fprintf('No beamforming pathloss = %.6f dB\n', pl_noBF);
outfile = 'pathloss_output.txt';
fid_out = fopen(outfile, 'w');
fprintf(fid_out, '%.6f\n', pluser1);
fclose(fid_out);

%{
fprintf('Task 3: Calculate SINR of two concurrent beams\n\n');

intf_fromuser2 = tx_gain2(user1Idx);
plfromuser2 = friis_equation(freq, intf_fromuser2, rx_gain, dist1);
recpowerfromuser2 = P_tx_dBm + plfromuser2;

fprintf('Interference at User1 (from User2 beam)\n');
fprintf('User1 sees Interference Gain = %.6f\n', intf_fromuser2);
fprintf('Interference Power from user2 = %.6f dBm\n\n', recpowerfromuser2);

I_mW = 10^(recpowerfromuser2/10);
N_mW = 10^(N0_dBm/10);
total_mW = I_mW + N_mW;
total_dBm = 10*log10(total_mW);
SINRuser1 = Rx_poweruser1 - total_dBm;

fprintf('User1 SINR\n');
fprintf('Signal Power (User1) = %.6f dBm\n', Rx_poweruser1);
fprintf('Rx1 interference power = %.6f dBm\n', total_dBm);
fprintf('Rx1 SINR = %.6f dB\n\n', SINRuser1);
%}


%{
% different antenna and codebook part
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% TA Section %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% DO NOT MODIFY THIS SECTION. Any changes will result in a penalty.
close all; clear; clc;
addpath ./ewa_function;
rng(77);

% Environment Configurations
freq = 24e9;
tx_node_number = 1;      % Number of Tx users
tx_antenna_number = 16;  % Number of Tx antennas
rx_node_number = 2;      % Number of Rx users
rx_antenna_number = 1;   % Number of Rx antennas
tx_beam_direction = 0:10:180; % degree
d = 0.5;                 % Distance between antennas (multiple of wavelength)
P_tx_dBm = 20;           % Transmission power of Tx (dBm)
N0_dBm = -88;            % Noise power (dBm)
tx_location = [0, 0];    % Tx location

% Random Rx location
rx_location = zeros(rx_node_number, 2);
for i = 1:rx_node_number
    r = 5 + 20 * rand();    % Random distance between 5 and 25 meters (m)
    angle = 180 * rand();
    x = r * cosd(angle);    % Beam direction with a small random offset
    y = r * sind(angle);    % Beam direction with a small random offset
    rx_location(i, :) = [x, y];
end

fprintf('Rx1 location: (%.2f, %.2f)\n', rx_location(1,1), rx_location(1,2));
fprintf('Rx2 location: (%.2f, %.2f)\n\n', rx_location(2,1), rx_location(2,2));

resolution = 360;           % Number of angles dividing 180 degrees
%%%%%%%%%%%%%%%%%%%%%%%%%%%%% TA Section End %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

theta_actual = atan2d(rx_location(:,2), rx_location(:,1));
codebook_cell = {0:5:180, 0:15:180};
antenna_opts = [4, 8];

angles = linspace(0.5, 180, resolution); 
angles_rad = deg2rad(angles);             
wVec = -2*pi*d .* cos(angles_rad);         

for user = 1:2
    theta_user = theta_actual(user);
    bp_conditions = cell(4,1);   
    opt_angles = zeros(4,1);     
    labels = cell(4,1);          
    cond_idx = 1;
    
    for ant = 1:length(antenna_opts)
        for cb = 1:length(codebook_cell)
            current_codebook = codebook_cell{cb};
            [~, idx] = min(abs(current_codebook - theta_user));
            theta_opt = current_codebook(idx);
            opt_angles(cond_idx) = theta_opt;
            
            step_size = current_codebook(2) - current_codebook(1);
            labels{cond_idx} = sprintf('%d antennas, codebook [%d:%d:180], optimal beam %.1f°', ...
                antenna_opts(ant), current_codebook(1), step_size, theta_opt);
            
            [a_steer, ~] = uniform(d, theta_opt, antenna_opts(ant));
            w = a_steer;
            
            X = dtft(w, wVec);
            bp = abs(X).^2;
            bp_conditions{cond_idx} = bp;
            cond_idx = cond_idx + 1;
        end
    end
    
    figure;
    hold on;
    for i = 1:4
        plot(angles, bp_conditions{i}, 'LineWidth', 2);
    end
    xlabel('Angle (degrees)');
    ylabel('Tx Gain');
    title(sprintf('User %d Optimal Beam Pattern', user));
    legend(labels, 'Location', 'Best');
    grid on;
    hold off;

    figure;
    pax = polaraxes;         
    hold(pax, 'on');          
    for i = 1:4
        polarplot(pax, angles_rad, bp_conditions{i}, 'LineWidth', 2);
    end
    title(pax, sprintf('User %d Optimal Beam Pattern', user));
    legend(pax, labels, 'Location', 'Best');
    hold(pax, 'off');
end
%}



% random 20 times part
%{
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% TA Section %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% DO NOT MODIFY THIS SECTION. Any changes will result in a penalty.
close all; clear; clc;
addpath ./ewa_function;
 
% Environment Configurations
freq = 24e9;
tx_node_number = 1;      % Number of Tx users
tx_antenna_number = 16;  % Number of Tx antennas
rx_node_number = 2;      % Number of Rx users
rx_antenna_number = 1;   % Number of Rx antennas
tx_beam_direction = 0:10:180; % degree
d = 0.5;                 % Distance between antennas (multiple of wavelength)
P_tx_dBm = 20;           % Transmission power of Tx (dBm)
N0_dBm = -88;            % Noise power (dBm)
tx_location = [0, 0];    % Tx location

resolution = 360;           % Number of angles dividing 180 degrees
%%%%%%%%%%%%%%%%%%%%%%%%%%%%% TA Section End %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

results = zeros(20, 4);
for run = 1:20
    rx_location = zeros(rx_node_number, 2);
    for i = 1:rx_node_number
        r = 5 + 20 * rand();    
        angle_temp = 180 * rand();
        x = r * cosd(angle_temp);
        y = r * sind(angle_temp);
        rx_location(i, :) = [x, y];
    end
   
    theta_actual = atan2d(rx_location(:,2), rx_location(:,1)); 
    [~, idx1] = min(abs(tx_beam_direction - theta_actual(1)));
    [~, idx2] = min(abs(tx_beam_direction - theta_actual(2)));
    
    theta_optimal(1) = tx_beam_direction(idx1);
    theta_optimal(2) = tx_beam_direction(idx2);
    
    [a_steer_rx1, ~] = uniform(d, theta_optimal(1), tx_antenna_number);
    [a_steer_rx2, ~] = uniform(d, theta_optimal(2), tx_antenna_number);
    
    wUser1 = a_steer_rx1;
    wUser2 = a_steer_rx2;
    
    angles = linspace(0.5, 180, resolution);
    angles_rad = deg2rad(angles);   
    
    wVec = -2*pi * d .* cos(angles_rad);
    X1 = dtft(wUser1, wVec);          
    tx_gain1 = abs(X1).^2;            
    X2 = dtft(wUser2, wVec);
    tx_gain2 = abs(X2).^2;

    [~, user1Idx] = min(abs(angles - theta_actual(1)));
    tx_gainuser1 = tx_gain1(user1Idx);  
    rx_gain = 1;                         
    dist1 = norm(rx_location(1,:));
    
    pluser1 = friis_equation(freq, tx_gainuser1, rx_gain, dist1);
    Rx_poweruser1 = P_tx_dBm + pluser1;
    SNRuser1 = Rx_poweruser1 - N0_dBm;

    intf_fromuser2 = tx_gain2(user1Idx);
    plfromuser2 = friis_equation(freq, intf_fromuser2, rx_gain, dist1);
    recpowerfromuser2 = P_tx_dBm + plfromuser2;
    
    I_mW = 10^(recpowerfromuser2/10);
    N_mW = 10^(N0_dBm/10);
    total_mW = I_mW + N_mW;
    total_dBm = 10*log10(total_mW);
    SINRuser1 = Rx_poweruser1 - total_dBm;
 
    results(run, :) = [Rx_poweruser1, SNRuser1, total_dBm, SINRuser1];
end
average_results = mean(results, 1);
fprintf('Averaged Results over 20 runs:\n');
fprintf('Average Rx1 power            = %.6f dBm\n', average_results(1));
fprintf('Average Rx1 SNR              = %.6f dB\n', average_results(2));
fprintf('Average Rx1 interference pwr = %.6f dBm\n', average_results(3));
fprintf('Average Rx1 SINR             = %.6f dB\n', average_results(4));
%}
