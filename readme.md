# Network System Capstone Labs — Outline Summary

This document provides an outline summary of Lab1–Lab6, focusing on what was implemented in each lab.

## Lab1 — Analog Beamforming (Matlab)
- Implement beam scanning to find optimal beam angles for users
- Calculate Tx gain and apply Friis model to obtain Rx power and SNR
- Simulate concurrent beams to compute interference power and SINR

## Lab2 — Beamforming with NS‑3 (LEO)
- Configure satellite and ground station topology in NS‑3
- Import Tx gain and pathloss results from Lab1 to compute Rx power and SNR
- Set data rate based on SNR, simulate transmissions, and measure end-to-end delay

## Lab3 — Network Optimization with OR‑Tools
- Generate link data rate graph (network.graph) for ground station–satellite connections
- Build ILP optimization model to minimize job completion time
- Implement Greedy baseline algorithm and compare results with OR‑Tools solution

## Lab4 — Satellite-Ground Station Simulation (NS‑3)
- Simulate transmissions in NS‑3 using association results from Lab3
- Implement packet sending/receiving tracking to record start and finish times per ground station
- Output satellite collection times and overall collection completion time

## Lab5 — Load Balancing Routing
- Perform routing for multiple source–destination pairs to maximize total throughput
- Complete ILP model for optimal solution
- Design and implement MyAlgo (heuristic algorithm), compare performance against baseline

## Lab6 — Load Balance Transmission with NS‑3
- Modify Nix-Vector-Routing to support user-defined routing paths
- Simulate packet transmissions and trace hop-by-hop timestamps at MAC layer to verify path correctness
- Compare throughput of two different path sets and analyze congestion effects

---
> Each lab focuses on different aspects: Lab1–2 emphasize antenna and link-level calculations, Lab3–5 focus on optimization modeling and algorithm design, and Lab6 validates performance in simulation.
