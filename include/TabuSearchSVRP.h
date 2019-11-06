#include "SVRP.h"

class TabuSearchSVRP {

public:

    Graph g;
    double penalty, bestPenalizedSol, bestSol;
    int numVehicles, capacity, numSelected, numNearest;
    int itCount, numInfeasibleNearby;
    int currNoImprovement, maxNoImprovement;
    vector<int> relativeDemand;
    vector<vector<int>> routes, closestNeighbours;

    // Etapas
    TabuSearchSVRP(Graph g, int numVehicles, int capacity);
    void neighbourhoodSearch();
    void incumbentUpdate();
    void coefficientUpdate();
    void intensification();

    // Funções
    double penalizedExpectedLength(int solVehicles);

};