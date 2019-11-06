#include "graph.h"

vector<vector<double>> probTotalDemand(Graph g, vector<int> route);
double probReachCapacity(int i, Graph g, vector<vector<double>> f, int capacity, vector<int> route);
double probExceedsCapacity(int i, Graph g, vector<vector<double>> f, int capacity, vector<int> route);
double returnCost (int i, int j, Graph g, vector<int> orderInRoute);
double routeExpectedLength(Graph g, vector<vector<double>> f, int capacity, vector<int> route);
double totalExpectedLength(Graph g, int capacity, vector<vector<int>> routes);
vector<vector<int>> randomRoutes(int numberVertices, int numberVehicles);