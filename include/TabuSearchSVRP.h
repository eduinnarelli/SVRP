#include "SVRP.h"

class TabuSearchSVRP {

public:

    Graph g;
    double penalty;
    int numVehicles, numSelected, numNearest;
    int itCount, numInfeasibleNearby;
    int currNoImprovement, maxNoImprovement;
    vector<int> relativeDemand;
    vector<vector<int>> routes, closestNeighbours;

    // Etapas
    TabuSearchSVRP(Graph g, int numVehicles);
    void neighbourhoodSearch();
    void incumbentUpdate();
    void coefficientUpdate();
    void intensification();

    // Funções
    double penalizedExpectedLength();

};