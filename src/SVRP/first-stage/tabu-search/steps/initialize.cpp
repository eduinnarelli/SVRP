#include "SVRP.h"

/* Etapa 1: construir solução e estruturas iniciais */
void TabuSearchSVRP::initialize(Graph inst, int numVehicles, int capacity)
{

    if (verbosity == 'y')
        cout << endl
             << "INITIALIZE" << endl;

    this->g = inst;
    this->numVehicles = numVehicles;
    this->capacity = capacity;

    int h = min(this->g.numberVertices - 1, 10);

    this->closestNeighbours.clear();

    // Computar os h vizinhos mais próximos de cada vértice
    for (int i = 0; i < this->g.numberVertices; i++) {

        vector<int> closest(this->g.numberVertices - 1); // -1, pois não pegamos o depósito
        vector<double> dist = this->g.adjMatrix[i];

        iota(closest.begin(), closest.end(), 1); // Com 0 pegamos o depósito

        sort(closest.begin(), closest.end(), [&dist](size_t i1, size_t i2) {
            return dist[i1] < dist[i2];
        });

        closest.erase(closest.begin()); // Desconsideramos o vértice como seu próprio vizinho
        closest.erase(closest.begin() + h, closest.end());

        this->closestNeighbours.push_back(closest);
    }

    this->routeOfClient.resize(this->g.numberVertices);
    this->sol.routes.clear();

    // Rotas de ida e volta ao depósito
    for (int i = 1; i < this->g.numberVertices; i++) {

        vector<int> route(1, i);
        this->sol.routes.push_back(route);
        this->routeOfClient[i] = i - 1;
    }

    this->penalty = 1;
    this->numRoutes = this->g.numberVertices - 1;

    this->sol.expectedCost = penalizedExpectedLength(this->sol.routes);
    this->bestPenalExpCost = this->sol.expectedCost;

    if (verbosity == 'y')
        cout << "Demandas relativas:" << endl;

    // Coeficiente que relaciona a demanda esperada do vértice com a total
    for (int i = 1; i < this->g.numberVertices; i++) {

        relativeDemand.push_back(this->g.expectedDemand[i] / this->g.totalExpectedDemand);

        if (verbosity == 'y')
            cout << relativeDemand[i - 1] << endl;
    }

    // Ajuste de parâmetros
    this->numNearest = min(this->g.numberVertices - 1, 5);
    this->numSelected = min(this->g.numberVertices - 1, 5 * this->numVehicles);

    this->itCount = 0;
    this->currNoImprovement = 0;

    if (numVehicles < this->g.numberVertices - 1) {
        this->numInfeasibleNearby = 1;
        this->bestFeasibleSol.expectedCost = numeric_limits<double>::max();
    }
    else {
        this->numInfeasibleNearby = 0;
        if (numVehicles == this->g.numberVertices - 1) {
            this->bestFeasibleSol.expectedCost = this->sol.expectedCost;
            this->bestFeasibleSol.routes = this->sol.routes;
        }
        else {
            this->bestFeasibleSol.routes.clear();
            this->bestFeasibleSol.expectedCost = numeric_limits<double>::max();
        }
    }

    this->maxNoImprovement = 50 * this->g.numberVertices;

    if (verbosity == 'y')
        cout << "END_INITIALIZE" << endl
             << endl;
}
