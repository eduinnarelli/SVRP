#include "graph.h"

vector<vector<double> > probDemandsInRoute(Graph g, vector<int> route);
double probReachCapacity(int i, Graph g, vector<vector<double>> f, int capacity, vector<int> route);
double probExceedsCapacity(int i, Graph g, vector<vector<double>> f, int capacity, vector<int> route);
double custoRetorno (int i, int j, Graph g, vector<int> orderInRoute);
double expectedLength(Graph g, vector<vector<double>> f, int capacity, vector<int> route);
