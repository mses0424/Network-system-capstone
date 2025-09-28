# Network System Capstone Labs — Outline Summary

本文件以大綱方式統整 Lab1～Lab6「做了什麼事情」，快速了解每次實驗的重點。

## Lab1 — Analog Beamforming (Matlab)
- 實作波束掃描，找出使用者的最佳波束角度
- 計算 Tx 增益並套用 Friis 模型得到 Rx 功率與 SNR
- 模擬兩束同時傳輸，計算干擾功率與 SINR

## Lab2 — Beamforming with NS‑3 (LEO)
- 在 NS‑3 中設定衛星與地面站拓樸
- 匯入 Lab1 的 Tx 增益與 Pathloss，計算 Rx power 與 SNR
- 根據 SNR 設定資料率並模擬傳輸，量測端到端延遲

## Lab3 — Network Optimization with OR‑Tools
- 產生地面站與衛星連結的資料率圖 (network.graph)
- 以 ILP 建立最佳化模型，決定關聯方式以最小化完成時間
- 實作貪婪演算法作為基線，比較 OPT 與 Greedy 結果

## Lab4 — Satellite-Ground Station Simulation (NS‑3)
- 根據 Lab3 的關聯結果，在 NS‑3 中模擬實際傳輸
- 實作封包傳送與接收紀錄，追蹤每個地面站的起迄時間
- 輸出各衛星的收集時間與總收集完成時間

## Lab5 — Load Balancing Routing
- 在圖上為多組來源–目的對進行路由，最大化總吞吐
- 使用 ILP 模型完成最佳化解
- 設計並實作 MyAlgo（啟發式演算法），比較效能與基線

## Lab6 — Load Balance Transmission with NS‑3
- 修改 Nix-Vector-Routing，使其能讀取使用者自訂路徑
- 模擬封包傳輸並在 MAC 層追蹤逐跳時間戳，驗證路徑正確
- 比較兩組不同路徑配置的總吞吐量，分析壅塞效應

---
> 各 Lab 聚焦於不同面向：Lab1–2 在於天線與鏈路層計算，Lab3–5 在於最佳化模型與演算法設計，Lab6 回到模擬環境驗證效能。
