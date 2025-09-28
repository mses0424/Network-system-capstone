# Lab1: Analog Beamforming

This lab implements analog beamforming simulation in Matlab by modifying **bf.m**.  

## Tasks

### Task 1: Simulate Beam Scanning
- Calculate the actual AoD (θ1, θ2) for two users.  
- Find the closest beam angle from the codebook `[0:10:180]`.  
- Identify the optimal beam direction θ1* and θ2* that maximize Tx gain.  

### Task 2: Calculate SNR
- Find the Tx gain for user 1 at the optimal beam θ1*.  
- Compute Rx power and SNR (in dB) using Friis’ free space model.  
  - Assume Rx gain = 1  
  - Use 360 resolution (180° → 360 angles).  

### Task 3: Calculate SINR
- Find the Tx gain for user 2 at the optimal beam θ2*.  
- Compute interference power and SINR (in dB) for user 1.  
  - Assume Rx gain = 1  
  - Use Friis’ free space model.  
  - Convert power values to dBm.  

---
## File to Modify
- `bf.m`

## Parameters
- Carrier frequency: 24 GHz  
- Tx power: 20 dBm  
- Noise power: -88 dBm  
- Tx: 16-antenna linear phased array  
- Rx: single-antenna receiver  
- Codebook size: `[0:10:180]`  
