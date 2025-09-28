# Lab6: Load Balance Transmission with NS-3

This lab uses NS-3 to simulate load balance transmissions with user-defined routing paths. The goal is to set custom paths, trace packet transmissions, and compare throughput of different path sets.  

## Tasks

### Task 1: Modify Nix-Vector-Routing
- Modify NVR package to use user-defined paths instead of default BFS.  
- **Steps:**  
  - Task 1.1: Add `SetPathFile()` in `NixVectorHelper` and `SetPaths()` in `NixVectorRouting` to read paths from an input file (e.g., `paths1.in`, `paths2.in`).  
  - Task 1.2: Modify `GetNixVector()` to build routes according to the Table.  
  - Task 1.3: Disable cache by editing `GetNixVectorInCache()` and `GetIpRouteInCache()`.  

**Path File Format:**  
```
srcId dstId pathLength node1 node2 ... nodeN
36 38 5 36 3 2 5 38
```

### Task 2: Send & Trace Packets
- Implement `SendPacket()` using `BulkSendHelper` and `PacketSinkHelper`.  
  - Use TCP protocol.  
  - MaxBytes = 512, SendSize = 512.  
- Call `SendPacket()` in `main()` (srcId=36, dstId=38).  
- Implement `EchoMacTxRx()` to log send/receive timestamps along the path.  
- Output format example:  
```
MacTx at node: 36, now: +1.17536e+09ns
MacRx at node: 3, now: +1.66518e+09ns
...
MacRx at node: 38, now: +2.17852e+09ns
```

### Task 3: Calculate Throughput
- Compare two different path sets (`paths1.in`, `paths2.in`).  
- **Steps:**  
  - Task 3.1: Continuously send packets (MaxBytes = 0).  
  - Task 3.2: Call `SendPacket()` for 3 SD pairs:  
    - 36→38  
    - 37→40  
    - 39→41  
  - Task 3.3: Use `GetTotalRx()` to calculate throughput.  
- Output format example:  
```
36->38: 12345
37->40: 87878
39->41: 13589
Total throughput: 113812
```

---
## Files to Modify / Generate
- `leo-lab6.cc`  
- `nix-vector-routing/helper/nix-vector-helper.h`  
- `nix-vector-routing/helper/nix-vector-helper.cc`  
- `nix-vector-routing/model/nix-vector-routing.h`  
- `nix-vector-routing/model/nix-vector-routing.cc`  
- `paths1.in`, `paths2.in`  
- `task2.out`, `task3.paths1.out`, `task3.paths2.out`  

## Input / Output
- Input: `paths1.in`, `paths2.in`  
- Output: `task2.out`, `task3.paths1.out`, `task3.paths2.out`  
