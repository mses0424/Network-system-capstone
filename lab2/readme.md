# Lab2: Beamforming with NS3

This lab implements beamforming in NS3 by integrating results from Lab1.  

## Tasks

### Task 1: Topology Configuration
- Modify `calculate_delay.cc`  
- Set Tx, Rx, Satellite coordinates  
- Convert to (x, y, z) using `MobilityModel`  
- Output positions to `.txt` file  

### Task 2: Calculate Tx Gain
- Modify `bf.m`  
- Load `.txt` file with positions  
- Use codebook `[0:5:90]`, convert horizontal → elevation angle  
- Update antenna phase offset `psi = 2 * pi * d * sin(theta)`  
- Compute Tx gain and pathloss using Friis model  
- Output pathloss to `.txt`  

### Task 3: Calculate Rx Power
- Modify `leo-propagation-loss-model.h` and `.cc`  
  - Make `DoCalcRxPower()` public  
  - Load `.txt` pathloss and compute Rx power  
- Modify `propagation-loss-model.h` (inheritance public)  
- Modify `mock-channel.cc` (line 187 condition `rxPower < -60.0`)  

### Task 4: Transmission Configuration
- Modify `calculate_delay.cc`  
- Transmission parameters:  
  - Bandwidth = 2 MHz  
  - Noise = -110 dBm  
  - Tx Power = 105.9 dBm  
- Call `DoCalcRxPower()` to get Rx power  
- Calculate SNR (convert to linear)  
- Compute achievable data rate (Shannon theorem)  

### Task 5: Compute End-to-End Delay
- Modify `calculate_delay.cc`  
- Update DataRate with calculated value using `setGndDeviceAttribute()`  
- Output average end-to-end delay  

---
## Files to Modify
- `calculate_delay.cc`  
- `bf.m`  
- `leo-propagation-loss-model.h` / `.cc`  
- `propagation-loss-model.h`  
- `mock-channel.cc`  

## Parameters
- Bandwidth: 2 MHz  
- Tx Power: 105.9 dBm  
- Noise: -110 dBm  
- Codebook: `[0:5:90]`  
