#include "TabuSearchSVRP.h"
#include <list>

vector<vector<double>> probTotalDemand(Graph g, vector<int> route);

double probReachCapacity(int i, Graph g, vector<vector<double>> f, int capacity, vector<int> route);

double probExceedsCapacity(int i, Graph g, vector<vector<double>> f, int capacity, vector<int> route);

double returnCost (int i, int j, Graph g, vector<int> orderInRoute);

vector<vector<int>> randomRoutes(int numberVertices, int numberVehicles);

void drawRoutes(Graph g, svrpSol solution, string nameOutputFile);

vector<list<int>> createAdjList(vector<edge> edges, int n);

vector<vector<int>> connectedComponents(vector<list<int>> adjList, int n);

void DFSComponent(int v, vector<list<int>> adjList, vector<bool> &visited, vector<int> &component);

namespace Bertsimas2ndStage {
    double routeExpectedLength(Graph g, vector<vector<double>> f, int capacity, vector<int> route);
    double totalExpectedLength(Graph g, int capacity, vector<vector<int>> routes);
}

namespace BruteForce2ndStage {
    double scenaryCost(vector<vector<int>> A, vector<int> B, Graph g, vector<int> route, int capacity);
    double totalExpectedLength(Graph g, int capacity, vector<vector<int>> route);
    double routeExpectedLength(Graph g, int capacity, vector<int> route);
}