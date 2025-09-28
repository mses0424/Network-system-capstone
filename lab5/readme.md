# Lab5: Load Balancing Routing with OR-Tools and Custom Algorithm

This lab focuses on load balancing routing. Given a network topology and a set of source-destination (SD) pairs, the goal is to assign each SD pair a routing path such that the total throughput is maximized.  

## Tasks

### Task 1: Complete the ILP Model
- Add 3 constraints to the ILP formulation:  
  1. **Y-Z binding**: A link is used if assigned to any SD pair.  
  2. **Single transmitter per node**: Each node can use at most one outgoing link.  
  3. **Single receiver per node**: Each node can use at most one incoming link.  
- Also include constraints for link capacity, flow conservation, and single-path restriction.  

### Task 2: OR-Tools Program
- Implement the ILP model in **lab5_ortools.cc**.  
- Input: `network.graph`  
- Output: `network.ortools.out`  
- The objective is to maximize total throughput.  
- Multiple optimal solutions may exist, but the objective value must match.  

### Task 3: Design & Implement MyAlgo
- Design a polynomial-time heuristic algorithm (`lab5_myalgo.cpp`) that satisfies all ILP constraints.  
- Output must be feasible and yield better average throughput than the given baseline.  
- Baseline algorithm: shortest-path (BFS) routing with capacity updates.  
- Input: `network.graph`  
- Output: `network.myalgo.out`  
- Submit to Codeforces for correctness verification.  

---
## Files to Modify / Generate
- `lab5_ortools.cc`  
- `lab5_myalgo.cpp`  
- `network.ortools.out`  
- `network.myalgo.out`  

## Input / Output
- Input: `network.graph`  
- Output: `network.ortools.out`, `network.myalgo.out`  
