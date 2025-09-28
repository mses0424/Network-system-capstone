#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>
#include <map>
#include <tuple>
#include <queue>
#include <algorithm>
#include <iomanip>
#include "absl/flags/flag.h"
#include "absl/log/flags.h"
#include "absl/strings/str_format.h"
#include "ortools/base/init_google.h"
#include "ortools/base/logging.h"
#include "ortools/init/init.h"
#include "ortools/linear_solver/linear_solver.h"

using namespace std;
using namespace operations_research;

const string STUDENT_ID = "111550136";

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int T; //第一行是總測資數
  if(!(cin >> T)) return 0;
  cout << STUDENT_ID << '\n';
  double sum_throughput = 0.0; //之後用來存總共throughput用來算平均

  for(int inst = 0; inst < T; ++inst){
    int N, M, P; //每組側資第一行是 node數,edge數,SD pair數
    cin >> N >> M >> P;

    vector<tuple<int,int,int>> edges;
    edges.reserve(2 * M); //圖是無向圖要正反邊都建立
    for(int i = 0; i < M; ++i){
      int u, v, c;  //起點,終點,throughput
      cin >> u >> v >> c;
      edges.emplace_back(u, v, c);
      edges.emplace_back(v, u, c);
    }

    vector<pair<int,int>> sd(P); //存SD pair
    for(int i = 0; i < P; ++i){
      cin >> sd[i].first >> sd[i].second;
    }

    unique_ptr<MPSolver> solver(    //建立 MILP Solver
        MPSolver::CreateSolver("CBC_MIXED_INTEGER_PROGRAMMING"));
    if (!solver) {
      cerr << "Cannot create CBC solver\n";
      return 1;
    }

    // 建立z,之後會設定 z[u,v] = 1 如果link(u<->v)有被用 
    map<pair<int,int>, MPVariable*> z;
    for(auto& e : edges){
      int u,v,c; tie(u,v,c) = e;
      z[{u,v}] = solver->MakeBoolVar(
          absl::StrFormat("z_%d_%d", u, v));
    }

    // 建立x,x[f,u,v]是SD pair 中u-v link的data rate
    // 建立y,y[f,u,v]是SD pair 中u-v這條link有沒有被用
    map<tuple<int,int,int>, MPVariable*> x, y;
    for(int f = 0; f < P; ++f){
      for(auto& e : edges){
        int u,v,c; tie(u,v,c) = e;
        x[{f,u,v}] = solver->MakeNumVar(
            0.0, static_cast<double>(c),
            absl::StrFormat("x_%d_%d_%d", f,u,v));
        y[{f,u,v}] = solver->MakeBoolVar(
            absl::StrFormat("y_%d_%d_%d", f,u,v));
        {  // x-c*y <= 0 得x<=c*y，如果y=0的話代表u-v link沒有被用，所以流量x必=0 
          MPConstraint* ct =  //有用的話x的流量最高就是c*1=c
            solver->MakeRowConstraint(-MPSolver::infinity(), 0.0);
          ct->SetCoefficient(x[{f,u,v}],  1.0);
          ct->SetCoefficient(y[{f,u,v}], -static_cast<double>(c));
        }
        // Y-Z binding
        {  // y-z <= 0, 得y<=z, z=1時y這個SD pair不一定有被用，所以可以=0,1
          MPConstraint* ct =  //z=0時，y這個SD pair也一定不能用這條link 所以=0
            solver->MakeRowConstraint(-MPSolver::infinity(), 0.0);
          ct->SetCoefficient(y[{f,u,v}],  1.0);
          ct->SetCoefficient(z[{u,v}],    -1.0);
        }
      }
    }

    for(int f = 0; f < P; ++f){ //對每個SD pair
      int s = sd[f].first, t = sd[f].second;
      for(int u = 0; u < N; ++u){ //對每個node
        if(u == s || u == t) continue; //如果node是source或destination 之後再考慮
        MPConstraint* ct = solver->MakeRowConstraint(0.0, 0.0);
        for(auto& e : edges){ // 對每條邊
          int uu,vv,cc; tie(uu,vv,cc) = e; //把每條邊存到uu,vv,cc
          // x[{f,uu=u,vv}] 代表從u這個點流出的量(起點是u,終點是vv)，把全部從u這個點流出的量相加
          if (uu == u) ct->SetCoefficient(x[{f,uu,vv}],  1.0);
          // x[{f,uu,vv=u}] 代表從u這個點流入的量(起點是uu,終點是u)，把全部從u這個點流入的量相加
          if (vv == u) ct->SetCoefficient(x[{f,uu,vv}], -1.0);
          // 把全部流出的量減掉流入的量要=0
        }
      }
      {
        MPConstraint* ct = solver->MakeRowConstraint(0.0, 0.0);
        for(auto& e : edges){ 
          int uu,vv,cc; tie(uu,vv,cc) = e;
          //找所有邊如果有找到終點是source的,把x即流量全部相加要等於0，因source不會有流量流入
          if (vv == s) ct->SetCoefficient(x[{f,uu,vv}], 1.0);
        }
      }
      {
        MPConstraint* ct = solver->MakeRowConstraint(0.0, 0.0);
        for(auto& e : edges){
          int uu,vv,cc; tie(uu,vv,cc) = e;
          //找所有邊如果有找到起點是destination的,把x即流量全部相加要等於0，因destination不會有流量流出
          if (uu == t) ct->SetCoefficient(x[{f,uu,vv}], 1.0);
        }
      }
    }

    for(auto& e : edges) {
      int u,v,c; tie(u,v,c) = e;
      MPConstraint* ct = solver->MakeRowConstraint(
          0.0, static_cast<double>(c));
          // 設立範圍是 -無限大到c
      for(int f = 0; f < P; ++f){
        // 對每個SD pair 把u-v這條link的流量全部相加不可以大於c
        // 即u-v link在全部的情形下流量不可以超過c(上限)
        // 但基本上只會有一條路徑通過，所以會把流量全部分給經過的那條
        ct->SetCoefficient(x[{f,u,v}], 1.0);
      }
    }

    // Single transmitter per node, Single receiver per node
    for(int f = 0; f < P; ++f){ //對所有SD pair
      for(int u = 0; u < N; ++u){ //對所有node
        MPConstraint* ct = solver->MakeRowConstraint(
            0.0, 1.0);
            //範圍是 負無限大到1
        for(auto& e : edges){
          int uu,vv,cc; tie(uu,vv,cc) = e;
          if(uu == u){ //找所有起點是u的邊
            // y[{f,uu=u,vv}], 全部相加要小於1, 即全部從這個點流出的link數量要小於等於1
            ct->SetCoefficient(y[{f,uu,vv}], 1.0);
          }
        }
      }
    }
    for(int f = 0; f < P; ++f){
      for(int v = 0; v < N; ++v){
        MPConstraint* ct = solver->MakeRowConstraint(
            0.0, 1.0);
        for(auto& e : edges){
          int uu,vv,cc; tie(uu,vv,cc) = e;
          if(vv == v){ //跟上面同樣判斷 y[{f,uu,vv=v}], 全部相加要小於1
            // 從v這個點流入的link數量要小於等於1
            ct->SetCoefficient(y[{f,uu,vv}], 1.0);
          }
        }
      }
    }
    // 跟上面一樣的方法，對於每個點把流出與流入的link數量要小於等於1，只是改成操作z
    // Single transmitter per node, Single receiver per node
    for(int u = 0; u < N; ++u){
      MPConstraint* ct = solver->MakeRowConstraint(
          0.0, 1.0);
      for(auto& e : edges){
        int uu,vv,cc; tie(uu,vv,cc) = e;
        if(uu == u){
          ct->SetCoefficient(z[{uu,vv}], 1.0);
        }
      }
    }
    for(int v = 0; v < N; ++v){
      MPConstraint* ct = solver->MakeRowConstraint(
          0.0, 1.0);
      for(auto& e : edges){
        int uu,vv,cc; tie(uu,vv,cc) = e;
        if(vv == v){
          ct->SetCoefficient(z[{uu,vv}], 1.0);
        }
      }
    }

    // 只要看每組的source輸出的總量throughput全部相加讓他最高就好，因有設定中間點是進入流量等於輸出
    // 所以進到中間點的流量一定會找到其他點繼續流，直到流到終點
    MPObjective* obj = solver->MutableObjective();
    for(int f = 0; f < P; ++f){
      int s = sd[f].first;
      for(auto& e : edges){
        int uu,vv,cc; tie(uu,vv,cc) = e;
        if(uu == s){
          obj->SetCoefficient(x[{f,uu,vv}], 1.0);
        }
      }
    }
    obj->SetMaximization();

    auto status = solver->Solve();
    double total_rate = 0.0; // 把全部的把全部的throughput相加存起來，後面要算平均
    if (status == MPSolver::OPTIMAL || status == MPSolver::FEASIBLE) {
      total_rate = solver->Objective().Value();
    }
    sum_throughput += total_rate;

    vector<pair<int,int>> used_links; // 找出有使用的link，看z是不是1
    for (auto& pr : z) {  // 是的話把該組存出來
      if (pr.second->solution_value() > 0.5) used_links.push_back(pr.first);
    }
    cout << used_links.size() << '\n'; // 先輸出總共有幾個link被用在依序把有被用的印出
    for (auto& uv : used_links) cout << uv.first << ' ' << uv.second << '\n';

    for (int f = 0; f < P; ++f) { // 處理每一組SD pair
      int s = sd[f].first, t = sd[f].second;
      double rate = 0.0;
      for (auto& e : edges) {
        int u,v; double c;
        tie(u,v,c) = e; // 把該組SD pair 的datarate全部相加，如果是0的話就要輸出0 0
        if (u == s) rate += x[{f,u,v}]->solution_value();  // 代表沒有選用這組
      }
      if (rate < 1e-9) {
        cout << "0 0\n";
        continue;
      }

      vector<vector<int>> adj(N); // 建立一個表來表示有沒有相鄰
      for (auto& e : edges) {
        int u,v; double c;
        tie(u,v,c) = e; // 如果y=1的話代表u-v有使用，存進表中
        if (y[{f,u,v}]->solution_value() > 0.5) adj[u].push_back(v);
      }
      // 利用BFS回朔路徑，之後要印出每組SD pair經過的路徑
      vector<int> parent(N, -1);
      queue<int> q;
      q.push(s);
      parent[s] = s;
      while (!q.empty() && parent[t] == -1) {
        int u = q.front(); q.pop();
        for (int v : adj[u]) if (parent[v] == -1) {
          parent[v] = u;
          q.push(v);
        }
      }
      vector<int> path; // 放destination到source的路徑，最後加入source node在反轉得完整路徑
      for (int v = t; v != s; v = parent[v]) path.push_back(v);
      path.push_back(s);
      reverse(path.begin(), path.end()); 
      // 先印出 throughput 再把用幾個點，整個路徑印出
      cout << fixed << setprecision(6) << rate << ' ' << path.size();
      for (int node : path) cout << ' ' << node;
      cout << '\n';
    }
    //把每組SD pair的throughput印出
    cout << fixed << setprecision(6) << total_rate;
    if (inst < T-1) cout << '\n';
  }
  // 最後一行要算平均throughput
  cout << '\n' << fixed << setprecision(6) << (sum_throughput / T) << '\n';
  
  return 0;
}