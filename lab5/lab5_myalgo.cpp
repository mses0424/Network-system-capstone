#include <bits/stdc++.h>
#include <iomanip> 
using namespace std;
static const string STUDENT_ID = "111550136";

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T; // 第一行資料是有幾組側資
    if (!(cin >> T)) return 0;
    cout << STUDENT_ID << "\n";
    double sum_throughput = 0.0; // 存總throughput

    for (int inst = 0; inst < T; ++inst) {
        int N, M, P; // 每組側資的第一行是node,edge,SD pair數
        cin >> N >> M >> P;
        // 存u-v link流量
        vector<vector<double>> cap(N, vector<double>(N, 0.0));
        // 用來存這條u-v link還可以用多少flow，一開始是跟最大流量一樣
        vector<vector<double>> rem = cap;
        vector<vector<int>> adj(N); // 用來存u跟v有沒有相鄰
        for (int i = 0; i < M; ++i) {
            int u, v; // 把每條邊的u,v,c讀入，要雙向
            double c;
            cin >> u >> v >> c;
            cap[u][v] = rem[u][v] = c;
            cap[v][u] = rem[v][u] = c;
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        vector<pair<int, int>> sd(P);
        for (int i = 0; i < P; ++i) { // 存SD pair的起點終點
            cin >> sd[i].first >> sd[i].second;
        }
        // used_tx與used_rx用來存有沒有當過傳送端與接收端
        // 跟ortools的single tramsitter, receiver一樣
        vector<bool> used_tx(N, false), used_rx(N, false);
        // 存每組SD pair的最終流量
        vector<double> rates(P, 0.0);
        vector<vector<int>> paths(P);

        for (int f = 0; f < P; ++f) {
            int s = sd[f].first, t = sd[f].second;
            // best用來存從s開始到該點的路徑中最佳的throughput
            vector<double> best(N, 0.0);
            vector<int> parent(N, -1);
            // 起點預設為無限大
            best[s] = numeric_limits<double>::infinity();
            priority_queue<pair<double, int>> pq;
            pq.push({ best[s], s }); //先把(無限大,s)存進去

            while (!pq.empty()) {
                auto [b, u] = pq.top(); pq.pop();
                if (b < best[u]) continue; // 如果不一樣的話需跳過
                if (u == t) break; // 如果s=t代表到終點了，不用繼續看
                for (int v : adj[u]) { // 找所有與現在拿出的u點相鄰的點
                    // 拿出從現在的點到他的鄰居點剩餘多少flow可以用
                    double rcap = rem[u][v];
                    // 如果剩餘flow已經小於0，或u當過傳送端，v當過接收端，需跳過
                    // 因同一個點不可以當兩次接收或傳輸端
                    if (rcap <= 0 || used_tx[u] || used_rx[v]) continue;
                    double nb = min(b, rcap); // 拿出較小的代表是可以用的flow量
                    if (nb > best[v]) { // 有比走到的點原本的好的話做更新
                        best[v] = nb; //一開始best[v]會是0，所以會被更新成最大可以走的情形
                        parent[v] = u;
                        pq.push({ nb, v });
                    }
                }
            }

            double rate = best[t]; // 到終點可以用的throughput
            if (rate > 0) {
                vector<int> path;
                for (int v = t; v != s; v = parent[v])
                    path.push_back(v);
                path.push_back(s); // 建立路徑，先一路回朔最後加上source點 再反轉
                reverse(path.begin(), path.end());
                rates[f] = rate;  // 存throughput
                paths[f] = path;  // 存路徑
                for (int i = 0; i + 1 < (int)path.size(); ++i) {
                    int u = path[i], v = path[i + 1];
                    rem[u][v] -= rate; // 算完一組SD pair，要更新數值
                    used_tx[u] = true; // 更新剩下的流量為扣掉現在已經用的
                    used_rx[v] = true; // 更新u點已經當過傳送點，v點已經當過接收點
                }
            }
        }
        // 存所有真的有使用的link，後續要印出
        vector<pair<int, int>> used_links;
        for (int u = 0; u < N; ++u) {
            for (int v : adj[u]) {
                if (rem[u][v] < cap[u][v]) {
                    used_links.emplace_back(u, v);
                }
            }
        }
        // 每筆側資要先輸出link使用的數量，再輸出u-v link
        cout << used_links.size() << "\n";
        for (auto& e : used_links) {
            cout << e.first << " " << e.second << "\n";
        }
        // 計算總throughput
        double total_rate = 0.0;
        for (double r : rates) total_rate += r;
        // 輸出每組SD pair的結果
        for (int f = 0; f < P; ++f) {
            if (rates[f] <= 0) { // 如果沒有被採用，輸出0 0
                cout << "0 0\n";
            }
            else {
                auto& path = paths[f];
                //有被用的話，輸出throughput，用了幾個node跟路徑
                cout << static_cast<int>(rates[f]) << " " << path.size();
                for (int x : path) cout << " " << x;
                cout << "\n";
            }
        }
        // 輸出該組側資的throughput(我限制為整數)，如果不是最後一筆側資都要換行
        cout << static_cast<int>(total_rate) << (inst < T - 1 ? "\n" : "");
        sum_throughput += total_rate;
    }

    cout << "\n" << fixed << setprecision(6) << (sum_throughput / T) << "\n";
    return 0;
}
