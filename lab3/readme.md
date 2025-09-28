# Lab3: Network Optimization with OR-Tools

This lab uses OR-Tools to solve a satellite–ground station association optimization problem.  

You will first parse `network.pos` to calculate the data rate of each link (v, s) between ground stations and satellites.  
A link exists only if the received power is above the threshold.  

Using these data rates, you need to generate `network.graph`.  

Then, implement an Integer Linear Programming (ILP) model in OR-Tools to minimize the job completion time.  
The ILP should read `network.graph`, compute transmission times (1000 kb / data_rate), and output results to `network.ortools.out`.  
The output should include:  
- Maximum transmission time  
- Station-to-satellite associations  
- Per-satellite data collection times  

Next, implement a Greedy algorithm (`lab3_greedy.cc`) where each station connects to the satellite with the highest data rate.  
Output the result as `network.greedy.out`.  

Finally, compare the OR-Tools (OPT) and Greedy solutions in terms of:  
- Data collection time  
- Job completion time  
- Execution time  
- Advantages and disadvantages  

---
## Files
- To modify or generate:  
  `lab3_bipartite.cc`, `lab3_greedy.cc`, `lab3_ortools.cc`, `network.graph`, `network.greedy.out`, `network.ortools.out`  

- Input:  
  `network.pos`  

- Output:  
  `network.graph`, `network.greedy.out`, `network.ortools.out`  
