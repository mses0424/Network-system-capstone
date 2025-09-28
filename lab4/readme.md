# Lab4: Satellite-Ground Station Simulation with NS3

This lab uses NS-3 to simulate satellite–ground station association, based on the results from Lab3. The link data rate is set according to `network.graph` and association results (`network.ortools.out` or `network.greedy.out`). Packets are transmitted from each ground station to its associated satellite, and the total data collection time is calculated.  

## Tasks

### Task 1: Input File
- Read `network.graph` to obtain link data rates.  
- Read association results from `network.ortools.out` or `network.greedy.out`.  

### Task 2: SendPacket()
- Implement `SendPacket(int gsId, int satId)`.  
- Configure link data rate, satellite IP, and send packets using `BulkSendHelper` (TCP).  
- Parameters:  
  - MaxBytes = 125000  
  - SendSize = 512  
- Record the transmission start time.  
- Call `SendPacket()` in `main()` if the ground station hasn’t started and the satellite is idle.  

### Task 3: EchoRx()
- Complete `EchoRx()` to handle packet reception.  
- Record reception end time for each ground station.  
- Record satellite collection time after all its stations complete transmission.  
- Trigger `SendPacket()` for pending transmissions when satellite becomes idle.  

### Task 4: Output File
- Output results to `lab4.ortools.out` or `lab4.greedy.out`.  
- Format:  
  ```
  total_collection_time
  satellite_id collection_time
  ...
  ground_station_id trans_start_time recept_end_time
  ...
  ```
- Note: Collection times will differ from `network.xxx.out` due to protocol overhead.  

---
## Pre-process Steps
- Modify `leo-propagation-loss-model.cc`: comment out code in `DoCalcRxPower()` and return 0.  
- Add helper functions in `ground-node-helper.h` and `.cc` for constant position setting.  
- Download Lab4 repo and regenerate `network.ortools.out` and `network.greedy.out`.  

## Files to Modify / Generate
- `leo-lab4.cc`  
- `lab4.ortools.out`  
- `lab4.greedy.out`  

## Input / Output
- Input: `network.graph`, `network.ortools.out`, `network.greedy.out`  
- Output: `lab4.ortools.out`, `lab4.greedy.out`  
