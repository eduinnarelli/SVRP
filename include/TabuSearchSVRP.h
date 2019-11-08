#include "SVRP.h"

#define MAX_ITERATIONS 100000

struct routeMove {

    int client, neighbour, it;
    double approxCost;

    bool operator==(const routeMove& other) const {
        return this->client == other.client 
                && this->neighbour == other.neighbour;
    }

};

struct svrpSol {
    vector<vector<int>> routes;
    double expectedCost;
};

class TabuSearchSVRP {

    Graph g;
    double penalty, bestPenalExpCost;
    int numVehicles, capacity, numSelected, numNearest, numRoutes;
    int itCount, numInfeasibleNearby;
    int currNoImprovement, maxNoImprovement;
    routeMove moveDone;
    vector<int> routeOfClient;
    vector<double> relativeDemand;
    vector<vector<int>> closestNeighbours;
    vector<routeMove> tabuMoves;
    svrpSol sol, bestFeasibleSol;


public:

    svrpSol run(Graph inst, int numVehicles, int capacity);

private:

    // Etapas
    void initialize(Graph inst, int numVehicles, int capacity);
    void neighbourhoodSearch();
    void update();
    void intensification();

    // Funções
    double penalizedExpectedLength(vector<vector<int>> sol);
    double removalCost(int r, int client);
    double maxRemovalCost(int r);
    double approxInsertImpact(int a, int b, int c);
    double approxMoveCost(routeMove m, double relativeDemand);

};
