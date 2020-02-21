#include "SVRP.h"
#include "gurobi_c++.h"

void solveSVRP(Graph g, int m, int Q);
vector<vector<int>> buildRoutesFromSol(double **sol, int n);