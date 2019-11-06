#include <algorithm>
#include <numeric>
#include "TabuSearchSVRP.h"

// Etapa 1: Inicialização
TabuSearchSVRP::TabuSearchSVRP(Graph inst, int numVehicles, int capacity) {

    this->g = inst;
    this->numVehicles = numVehicles;
    this->capacity = capacity;

    int h = min(this->g.numberVertices - 1, 10);

    // Computar os h vizinhos mais próximos de cada vértice
    for (int i = 0; i < this->g.numberVertices; i++) {

        vector<int> closest(this->g.numberVertices);
        vector<double> dist = this->g.adjMatrix[i];

        iota(closest.begin(), closest.end(), 0);

        sort(closest.begin(), closest.end(), [&dist](size_t i1, size_t i2){
            return dist[i1] < dist[i2];
        });

        closest.erase(closest.begin());
        closest.erase(closest.begin() + h, closest.end());

        this->closestNeighbours.push_back(closest);

    }

    // Rotas de ida e volta ao depósito
    for (int i = 1; i < this->g.numberVertices; i++) {

        vector<int> route(1, i);
        this->routes.push_back(route);

    }

    this->penalty = 1;

    bestPenalizedSol = penalizedExpectedLength(this->g.numberVertices - 1);

    /* Coeficiente que relaciona a demanda esperada do vértice com a total
    Obs: precisa testar se tá certo mas coloquei os calculos de demanda esperada
    na inicialização do grafo pra aproveitar os loops de lá. */
    for (int i = 1; i < this->g.numberVertices; i++) {
        relativeDemand.push_back(this->g.expectedDemand[i] / this->g.totalExpectedDemand);
    }


    // Ajuste de parâmetros

    this->numNearest = min(this->g.numberVertices - 1, 5);
    this->numSelected = min(this->g.numberVertices - 1, 5*this->numVehicles);

    this->itCount = 0;
    this->currNoImprovement = 0;

    if (numVehicles < this->g.numberVertices - 1) {
        this->numInfeasibleNearby = 1;
        this->bestSol = numeric_limits<double>::max();
    } else {
        this->numInfeasibleNearby = 0;
        this->bestSol = this->bestPenalizedSol;
    }

    this->maxNoImprovement = 50*this->g.numberVertices;

}


double TabuSearchSVRP::penalizedExpectedLength(int solVehicles) {
    return totalExpectedLength(g, capacity, routes) + penalty*abs(solVehicles - numVehicles);
}