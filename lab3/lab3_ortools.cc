#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>
#include "absl/flags/flag.h"
#include "absl/log/flags.h"
#include "absl/strings/str_format.h"
#include "ortools/base/init_google.h"
#include "ortools/base/logging.h"
#include "ortools/init/init.h"
#include "ortools/linear_solver/linear_solver.h"

using operations_research::MPSolver;
using operations_research::MPVariable;
using operations_research::MPObjective;
using operations_research::LinearExpr;

namespace operations_research {

void Task2OrTools() {
  LOG(INFO) << "Google OR-Tools version: " << OrToolsVersion::VersionString();

  std::ifstream fin("network.graph");
  int num_stations, num_sats, num_links;
  fin >> num_stations >> num_sats >> num_links;
  const double DATA_UNIT = 1000.0; 

  std::vector<std::vector<double>> weight(num_stations, std::vector<double>(num_sats, -1.0));
  for (int e = 0; e < num_links; ++e) {
    int v, s;
    double rate_kbps;
    fin >> v >> s >> rate_kbps;
    weight[v][s] = DATA_UNIT / rate_kbps;
  }
  fin.close();

  std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("CBC_MIXED_INTEGER_PROGRAMMING"));
  std::vector<std::vector<const MPVariable*>> x(num_stations, std::vector<const MPVariable*>(num_sats, nullptr));
  for (int v = 0; v < num_stations; ++v) {
    for (int s = 0; s < num_sats; ++s) {
      if (weight[v][s] > 0) {
        x[v][s] = solver->MakeBoolVar(
            absl::StrFormat("x_%d_%d", v, s));
      }
    }
  }

  const MPVariable* T = solver->MakeNumVar(0.0, std::numeric_limits<double>::infinity(), "makespan");
  for (int v = 0; v < num_stations; ++v) {
    LinearExpr sum_x;
    for (int s = 0; s < num_sats; ++s) {
      if (x[v][s]) sum_x += x[v][s];
    }
    solver->MakeRowConstraint(sum_x == 1.0);
  }

  for (int s = 0; s < num_sats; ++s) {
    LinearExpr load;
    for (int v = 0; v < num_stations; ++v) {
      if (x[v][s]) {
        load += weight[v][s] * LinearExpr(x[v][s]);
      }
    }
    solver->MakeRowConstraint(load <= T);
  }

  MPObjective* const objective = solver->MutableObjective();
  objective->SetCoefficient(T, 1.0);
  objective->SetMinimization();


  const MPSolver::ResultStatus result_status = solver->Solve();
  LOG(INFO) << "Optimal makespan = " << T->solution_value();
  std::vector<int> assign(num_stations, -1);
  std::vector<double> sat_time(num_sats, 0.0);
  for (int v = 0; v < num_stations; ++v) {
    for (int s = 0; s < num_sats; ++s) {
      if (x[v][s] && x[v][s]->solution_value() == 1.0) {
        assign[v] = s;
        sat_time[s] += weight[v][s];
      }
    }
  }

  std::ofstream fout("network.ortools.out");
  fout << T->solution_value() << "\n";
  for (int v = 0; v < num_stations; ++v) {
    fout << v << " " << assign[v] << "\n";
  }
  for (int s = 0; s < num_sats; ++s) {
    fout << s << " " << sat_time[s] << "\n";
  }
}
} 

int main(int argc, char* argv[]) {
  InitGoogle(argv[0], &argc, &argv, true);
  absl::SetFlag(&FLAGS_stderrthreshold, 0);
  operations_research::Task2OrTools();
  return EXIT_SUCCESS;
}
