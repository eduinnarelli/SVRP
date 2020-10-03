#include "SVRP.h"

/*
routeExpectedLength: Calcula o custo esperado de uma rota.

Entrada:
g: grafo do problema sendo considerado;
f: matriz n por (20*(n-1))+1, onde n é o número de vértices do grafo.
f[m][r] = probabilidade da demanda total dos clientes 1, ..., m ser igual a r.
capacity: capacidade máxima do veículo do problema.
orderInRoute: ordem dos vértices na rota. orderInRoute[2] = 3o cliente da rota.

Saída: double indicando o custo esperado de se percorrer uma rota dada.
*/
double Bertsimas2ndStage::routeExpectedLength(
    Graph g,
    int capacity,
    vector<int> orderInRoute1)
{

    double expectedLengthOrientation1 = 0, expectedLengthOrientation2 = 0;
    vector<int> orderInRoute2 = orderInRoute1;
    reverse(orderInRoute1.begin(), orderInRoute1.end());
    vector<vector<double> > f1 = probTotalDemand(g, orderInRoute1);
    vector<vector<double> > f2 = probTotalDemand(g, orderInRoute2);
    int sizeRoute = orderInRoute1.size();

    /* Como um vértice pode não estar presente, computa-se o custo da possibilidade de cada
    vértice ser o primeiro,	o que é igual a ele estar presente e os anteriores não, vezes a
    sua distância ao depósito */
    for (int i = 0; i < sizeRoute; i++) {

        double expectedLength1 = g.adjMatrix[0][orderInRoute1[i]] * g.vertices[orderInRoute1[i]].probOfPresence;
        for (int r = 0; r <= i - 1; r++) {
            expectedLength1 *= (1 - g.vertices[orderInRoute1[r]].probOfPresence);
        }

        expectedLengthOrientation1 += expectedLength1;

        expectedLength1 = g.adjMatrix[0][orderInRoute2[i]] * g.vertices[orderInRoute2[i]].probOfPresence;
        for (int r = 0; r <= i - 1; r++) {
            expectedLength1 *= (1 - g.vertices[orderInRoute2[r]].probOfPresence);
        }

        expectedLengthOrientation2 += expectedLength1;
    }

    /* Como um vértice pode não estar presente, computa-se o custo de cada vértice ser o último,
	o que é igual a ele estar presente e os posteriores não estarem presentes, vezes a distância
	ao depósito */
    for (int i = 0; i < sizeRoute; i++) {

        double expectedLength2 = g.adjMatrix[orderInRoute1[i]][0] * g.vertices[orderInRoute1[i]].probOfPresence;
        for (int r = i + 1; r < sizeRoute; r++) {
            expectedLength2 *= (1 - g.vertices[orderInRoute1[r]].probOfPresence);
        }

        expectedLengthOrientation1 += expectedLength2;

        expectedLength2 = g.adjMatrix[orderInRoute2[i]][0] * g.vertices[orderInRoute2[i]].probOfPresence;
        for (int r = i + 1; r < sizeRoute; r++) {
            expectedLength2 *= (1 - g.vertices[orderInRoute2[r]].probOfPresence);
        }

        expectedLengthOrientation2 += expectedLength2;
    }

    /* Como um vértice pode não estar presente, computa-se o custo de cada possível próximo vértice
    da rota, o que é igual a dois vértices estarem presentes e os vértices entre eles não, vezes
    a distância entre esses vértices */
    for (int i = 0; i < sizeRoute; i++) {

        for (int j = i + 1; j < sizeRoute; j++) {

            double probBothPresent = g.vertices[orderInRoute1[i]].probOfPresence * g.vertices[orderInRoute1[j]].probOfPresence;
            double expectedLength3 = g.adjMatrix[orderInRoute1[i]][orderInRoute1[j]] * probBothPresent;
            for (int r = i + 1; r <= j - 1; r++) {
                expectedLength3 *= (1 - g.vertices[orderInRoute1[r]].probOfPresence);
            }

            expectedLengthOrientation1 += expectedLength3;

            probBothPresent = g.vertices[orderInRoute2[i]].probOfPresence * g.vertices[orderInRoute2[j]].probOfPresence;
            expectedLength3 = g.adjMatrix[orderInRoute2[i]][orderInRoute2[j]] * probBothPresent;
            for (int r = i + 1; r <= j - 1; r++) {
                expectedLength3 *= (1 - g.vertices[orderInRoute2[r]].probOfPresence);
            }

            expectedLengthOrientation2 += expectedLength3;
        }
    }

    /* Computar o custo esperado da capacidade limite ser atingida e o veículo retornar
    ao depósito. */
    for (int i = 0; i < sizeRoute; i++) {

        // Somar a probabilidade de exceder vezes o custo de retornar ao depósito
        double expectedLength4;
        if (i == 0) {
            expectedLength4 = 0;
        }
        else {
            expectedLength4 = probExceedsCapacity(i, g, f1, capacity, orderInRoute1) * returnCost(i, i, g, orderInRoute1);
        }

        expectedLengthOrientation1 += expectedLength4;

        // Somar a probabilidade de atingir exatamente a capacidade vezes o custo de retornar ao depósito
        for (int j = i + 1; j < sizeRoute; j++) {

            // Salvar probabilidade de atingir exatamente a capacidade no cliente i
            double probReachCap = probReachCapacity(i, g, f1, capacity, orderInRoute1);

            expectedLength4 = returnCost(i, j, g, orderInRoute1) * g.vertices[orderInRoute1[j]].probOfPresence;
            expectedLength4 *= probReachCap;
            for (int r = i + 1; r <= j - 1; r++) {
                expectedLength4 *= (1 - g.vertices[orderInRoute1[r]].probOfPresence);
            }

            expectedLengthOrientation1 += expectedLength4;
        }

        if (i == 0) {
            expectedLength4 = 0;
        }
        else {
            expectedLength4 = probExceedsCapacity(i, g, f2, capacity, orderInRoute2) * returnCost(i, i, g, orderInRoute2);
        }

        expectedLengthOrientation2 += expectedLength4;

        // Somar a probabilidade de atingir exatamente a capacidade vezes o custo de retornar ao depósito
        for (int j = i + 1; j < sizeRoute; j++) {

            // Salvar probabilidade de atingir exatamente a capacidade no cliente i
            double probReachCap = probReachCapacity(i, g, f2, capacity, orderInRoute2);

            expectedLength4 = returnCost(i, j, g, orderInRoute2) * g.vertices[orderInRoute2[j]].probOfPresence;
            expectedLength4 *= probReachCap;
            for (int r = i + 1; r <= j - 1; r++) {
                expectedLength4 *= (1 - g.vertices[orderInRoute2[r]].probOfPresence);
            }

            expectedLengthOrientation2 += expectedLength4;
        }
    }

    return min(expectedLengthOrientation1, expectedLengthOrientation2);
}
