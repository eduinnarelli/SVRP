#include "SVRP.h"

/*
totalExpectedLength: Calcula e acumula o custo esperado de todas as rotas.

Saída: double indicando o custo esperado de se percorrer todas as rotas.
*/
double Bertsimas2ndStage::totalExpectedLength(Graph g, int capacity, vector<vector<int> > routes)
{

    double totalExpLength = 0;

    for (int r = 0; r < routes.size(); r++) {

        if (routes[r].size() == 0)
            continue;

        double routeExpLength = Bertsimas2ndStage::routeExpectedLength(g, capacity, routes[r]);

        totalExpLength += routeExpLength;
    }

    return totalExpLength;
}
