close all; clear; clc;
addpath ./ewa_function;
rng(77);

tx_node_number = 1;      % Number of Tx users
tx_antenna_number = 16;  % Number of Tx antennas
rx_node_number = 2;      % Number of Rx users
rx_antenna_number = 1;   % Number of Rx antennas
tx_beam_direction = 0:5:90; % degree
d = 0.5; 
resolution = 180;

fid = fopen('network.pos', 'r');
line1 = textscan(fid, '%f %f %f %f %f', 1);
P_tx_dBm = line1{1};    
N0_dBm = line1{2};      
freq = line1{3};        
BW = line1{4};         
P_rx_th_dBm = line1{5}; 

line2 = textscan(fid, '%d %d', 1);
numGS = line2{1};
numSAT = line2{2};

GS_ids = cell(numGS,1);
GS_xyz = zeros(numGS,3);
for i = 1:numGS
    t = textscan(fid, '%s %f %f %f', 1);
    GS_ids{i} = t{1}{1};
    GS_xyz(i,:) = [t{2}, t{3}, t{4}];
end

SAT_ids = cell(numSAT,1);
SAT_xyz = zeros(numSAT,3);
for i = 1:numSAT
    t = textscan(fid, '%s %f %f %f', 1);
    SAT_ids{i} = t{1}{1};
    SAT_xyz(i,:) = [t{2}, t{3}, t{4}];
end
fclose(fid);
link_results = [];

for g = 1:numGS
    for s = 1:numSAT
        tx_location = GS_xyz(g,:);
        rx_location = SAT_xyz(s,:);
        
        dx = rx_location(1) - tx_location(1);
        dy = rx_location(2) - tx_location(2);
        d_h = sqrt(dx^2 + dy^2);
        dz = rx_location(3) - tx_location(3);
        theta_actual = abs(atand(d_h / dz));
        
        [~, idx_beam] = min(abs(tx_beam_direction - theta_actual));
        theta_optimal = tx_beam_direction(idx_beam);

        [a_steer, ~] = uniform(d, theta_optimal, tx_antenna_number);
        wUser = a_steer;
        angles = linspace(0.5, 90, resolution);
        angles_rad = deg2rad(angles);   
        wVec = -2*pi * d .* sin(angles_rad);
        X = dtft(wUser, wVec);
        tx_gain_pattern = abs(X).^2;

        [~, idx_theta_actual] = min(abs(angles - theta_actual));
        tx_gain = tx_gain_pattern(180 - idx_theta_actual);
        rx_gain = 1;
        dist = norm(rx_location - tx_location);
        pl_dB = friis_equation(freq, tx_gain, rx_gain, dist);
        Pr_dBm = P_tx_dBm + pl_dB;

        if Pr_dBm >= P_rx_th_dBm
            SNR_dB = Pr_dBm - N0_dBm;
            SNR_linear = 10^(SNR_dB / 10);
            C_bps = BW * log2(1 + SNR_linear);
            C_kbps = C_bps / 1e3;
            link_results = [link_results; {GS_ids{g}, SAT_ids{s}, C_kbps}];
        end
    end
end

fid_out = fopen('network.graph', 'w');
fprintf(fid_out, '%d %d %d\n', numGS, numSAT, size(link_results,1));
for k = 1:size(link_results,1)
    fprintf(fid_out, '%s %s %.6f\n', link_results{k,1}, link_results{k,2}, link_results{k,3});
end
fclose(fid_out);

