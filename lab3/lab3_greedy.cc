#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
#include "absl/flags/flag.h"
#include "absl/log/flags.h"
#include "absl/strings/str_format.h"
#include "ortools/base/init_google.h"
#include "ortools/base/logging.h"
#include "ortools/init/init.h"
#include "ortools/linear_solver/linear_solver.h"

int main(int argc, char* argv[]) {
  std::ifstream fin("network.graph");
  int num_stations, num_sats, num_links;
  fin >> num_stations >> num_sats >> num_links;

  std::vector<std::vector<double>> data_rate(num_stations, std::vector<double>(num_sats, -1.0));
  for (int e = 0; e < num_links; ++e) {
    int v, s;
    double rate_kbps;
    fin >> v >> s >> rate_kbps;
    data_rate[v][s] = rate_kbps;
  }
  fin.close();

  const double DATA_UNIT = 1000.0; 
  std::vector<int> assign(num_stations, -1);
  std::vector<double> sat_time(num_sats, 0.0);

  for (int v = 0; v < num_stations; ++v) {
    double best_rate = -1.0;
    int best_s = -1;
    for (int s = 0; s < num_sats; ++s) {
      if (data_rate[v][s] > best_rate) {
        best_rate = data_rate[v][s];
        best_s = s;
      }
    }
    assign[v] = best_s;
    double t = DATA_UNIT / best_rate;
    sat_time[best_s] += t;
  }

  double makespan = 0.0;
  for (double t : sat_time) {
    if (t > makespan) makespan = t;
  }

  std::ofstream fout("network.greedy.out");
  fout << makespan << "\n";
  for (int v = 0; v < num_stations; ++v) {
    fout << v << " " << assign[v] << "\n";
  }
  for (int s = 0; s < num_sats; ++s) {
    fout << s << " " << sat_time[s] << "\n";
  }
  fout.close();
  return EXIT_SUCCESS;
}
