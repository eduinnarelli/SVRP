#include "SVRP_1st_stage.h"

vector<vector<double> > probTotalDemand(Graph g, vector<int> route);
double probReachCapacity(int i, Graph g, vector<vector<double>> f, int capacity, vector<int> route);
double probExceedsCapacity(int i, Graph g, vector<vector<double>> f, int capacity, vector<int> route);
double returnCost (int i, int j, Graph g, vector<int> orderInRoute);
double expectedLength(Graph g, vector<vector<double>> f, int capacity, vector<int> route);