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
	vector<vector<double>> f, 
	int capacity, 
	vector<int> orderInRoute
) {

	double expectedLength = 0;
	int sizeRoute = orderInRoute.size();

	/* Como um vértice pode não estar presente, computa-se o custo da possibilidade de cada
	vértice ser o primeiro,	o que é igual a ele estar presente e os anteriores não, vezes a
	sua distância ao depósito */
	for (int i = 0; i < sizeRoute; i++) {

		double expectedLength1 = g.adjMatrix[0][orderInRoute[i]] * g.vertices[orderInRoute[i]].probOfPresence;
		for (int r = 0; r <= i - 1; r++) {
			expectedLength1 *= (1 - g.vertices[orderInRoute[r]].probOfPresence);
		}

		expectedLength += expectedLength1;
	}

	/* Como um vértice pode não estar presente, computa-se o custo de cada vértice ser o último,
	o que é igual a ele estar presente e os posteriores não estarem presentes, vezes a distância
	ao depósito */
	for (int i = 0; i < sizeRoute; i++) {

		double expectedLength2 = g.adjMatrix[orderInRoute[i]][0] * g.vertices[orderInRoute[i]].probOfPresence;
		for (int r = i + 1; r < sizeRoute; r++) {
			expectedLength2 *= (1 - g.vertices[orderInRoute[r]].probOfPresence);
		}

		expectedLength += expectedLength2;
	}

	/* Como um vértice pode não estar presente, computa-se o custo de cada possível próximo vértice
	da rota, o que é igual a dois vértices estarem presentes e os vértices entre eles não, vezes
	a distância entre esses vértices */
	for (int i = 0; i < sizeRoute; i++) {

		for (int j = i + 1; j < sizeRoute; j++) {

			double probBothPresent = g.vertices[orderInRoute[i]].probOfPresence * g.vertices[orderInRoute[j]].probOfPresence;
			double expectedLength3 = g.adjMatrix[orderInRoute[i]][orderInRoute[j]] * probBothPresent;
			for (int r = i + 1; r <= j - 1; r++) {
				expectedLength3 *= (1 - g.vertices[orderInRoute[r]].probOfPresence);
			}

			expectedLength += expectedLength3;
		}
	}

	/* Computar o custo esperado da capacidade limite ser atingida e o veículo retornar
	ao depósito. */
	for (int i = 0; i < sizeRoute; i++) {

		// Somar a probabilidade de exceder vezes o custo de retornar ao depósito
		double expectedLength4;
		if(i == 0) {
			expectedLength4 = 0;
		}
		else {
			expectedLength4 = probExceedsCapacity(i, g, f, capacity, orderInRoute) * returnCost(i, i, g, orderInRoute);
		}

		expectedLength += expectedLength4;

		// Somar a probabilidade de atingir exatamente a capacidade vezes o custo de retornar ao depósito
		for (int j = i + 1; j < sizeRoute; j++) {

			// Salvar probabilidade de atingir exatamente a capacidade no cliente i
			double probReachCap = probReachCapacity(i, g, f, capacity, orderInRoute);

			expectedLength4 = returnCost(i, j, g, orderInRoute) * g.vertices[orderInRoute[j]].probOfPresence;
			expectedLength4 *= probReachCap;
			for (int r = i + 1; r <= j - 1; r++) {
				expectedLength4 *= (1 - g.vertices[orderInRoute[r]].probOfPresence);
			}

			expectedLength += expectedLength4;
		}
	}

	return expectedLength;

}
