#include <algorithm>
#include <bitset>
#include <cmath>
#include <time.h>
#include "SVRP.h"

/*
probTotalDemand: Calcula todas as probabilidades de demanda total até cada vértice em uma
rota salvando estes valores em uma matriz. Nesta matriz, as linhas representam o último
vértice sendo considerado e as colunas a possível demanda até este vértice. Assim, se estamos
na 3a linha e 4a coluna, computamos a probabilidade de que a demanda dos vértices 1, 2 e 3
sejam iguais a 4. A ordem dos vértices segue a ordem da rota.

Entrada:
g: grafo do problema sendo considerado;
orderInRoute: vetor de inteiros que indica a ordem na qual os vértices são percorridos na rota.

Saída:
f: matriz n por (20*(n-1))+1, onde n é o número de vértices do grafo.
f[m][r] = probabilidade da demanda total dos clientes 1, ..., m é igual a r.

Observação: demanda máxima de um vértice é 20.
*/
vector<vector<double>> probTotalDemand(Graph g, vector<int> route) {

	// Inicializa a matriz com probabilidades 0
	int next, orderInRoute = 1, routeSize = route.size();
	vector<double> v(20*routeSize + 1, 0);
	vector<vector<double>> f(route.size() + 1, v);
	double probDemandK;

	f[0][0] = 1; // probabilidade da carga do veiculo até o depósito ser 0

	while (orderInRoute <= routeSize) {

		// Considera o próximo cliente da rota
		next = route[orderInRoute - 1];

		// Probabilidade de não haver nenhuma carga até o cliente, ou seja, todos ausentes
		f[orderInRoute][0] = (1 - g.vertices[next].probOfPresence)*f[orderInRoute-1][0];

		// Para todas as demandas até o cliente possíveis
		for (int dem = 1; dem <= 20 * orderInRoute; dem++) {

			// Probabilidade do cliente estar ausente, mantendo a mesma demanda anterior
			f[orderInRoute][dem] += (1-g.vertices[next].probOfPresence) * f[orderInRoute - 1][dem];

			// Para todas as demandas possíveis do cliente
			for (int k = 1; k <= min(20, dem); k++) {

				// Probabilidade do cliente estar presente com uma demanda k
				probDemandK = g.vertices[next].probOfPresence * g.vertices[next].probDemand[k];

				if (probDemandK > 0) {
					f[orderInRoute][dem] += probDemandK * f[orderInRoute - 1][dem - k];
				}
			}
		}
		orderInRoute++;
	}

	return f;
}

/*
probReachCapacity: Calcula a probabilidade da demanda até o vértice 'i' do grafo 'g' ser
exatamente igual a capacidade 'capacity'. Utiliza as probabilidades calculadas na função
probTotalDemand, a diferença é que esta função considera todos os números de possíveis
falhas, o que é equivalente a aumentar a capacidade.

Entrada:
i: vértice cuja probabilidade de demanda até ele é calculado;
g: grafo do problema sendo considerado;
f: matriz n por (20*(n-1))+1, onde n é o número de vértices do grafo.
f[m][r] = probabilidade da demanda total dos clientes 1, ..., m é igual a r.
capacity: capacidade máxima do veículo do problema.
orderInRoute: ordem dos vértices na rota. orderInRoute[2] = 3o cliente da rota.

Saída: double indicando a probabilidade da demanda até o vértice 'i' ser exatamente igual a
capacidade máxima do veículo.
*/
double probReachCapacity(int i, Graph g, vector<vector<double>> f, int capacity, vector<int> orderInRoute) {

	double probReachCap = 0;
	int vtx = orderInRoute[i];

	// Para todos os possíveis números de falhas "q"
	for (int q = 1; q <= floor((i + 1) * 20 / capacity); q++) {

		// Para todas as possíveis "k" capacidades residuais no vértice anterior a "i"
		for (int k = 1; k <= 20; k++) {
			probReachCap += g.vertices[vtx].probOfPresence * g.vertices[vtx].probDemand[k] * f[i][q * capacity - k];
		}
	}

	return probReachCap;
}

/*
probExceedsCapacity: Calcula a probabilidade da demanda até o vértice 'i' do grafo 'g' ser
maior do que a capacidade 'capacity'. Utiliza as probabilidades calculadas na função
probTotalDemand, a diferença é que esta função considera todos os números possíveis
de falhas, o que é equivalente a aumentar a capacidade.

Entrada:
i: vértice cuja probabilidade de demanda até ele é calculado;
g: grafo do problema sendo considerado;
f: matriz n por (20*(n-1))+1, onde n é o número de vértices do grafo.
f[m][r] = probabilidade da demanda total dos clientes 1, ..., m é igual a r.
capacity: capacidade máxima do veículo do problema.
orderInRoute: ordem dos vértices na rota. orderInRoute[2] = 3o cliente da rota.

Saída: double indicando a probabilidade da demanda até o vértice 'i' ser maior do que a
capacidade máxima do veículo.
*/
double probExceedsCapacity(int i, Graph g, vector<vector<double>> f, int capacity, vector<int> orderInRoute) {

	double probExceedsCap = 0, probDemandExceeds = 0;
	int vtx = orderInRoute[i];

	// Para todos os possíveis números de falhas "q"
	for (int q = 1; q <= floor((i + 1) * 20 / capacity); q++) {

		// Para todas as possíveis "k" capacidades residuais no vértice anterior a "i"
		for (int k = 1; k <= 19; k++) {

			// Para todas as possíveis demandas "r" do vértice "i" maiores do que "k"
			for (int r = k + 1; r <= 20; r++) {
				probDemandExceeds += g.vertices[vtx].probOfPresence * g.vertices[vtx].probDemand[r];
			}

			/* Somar probabilidade de que a demanda em "i" é maior do que "k"
			e que a demanda anterior a "i" seja igual a q*capacity-k */
			probExceedsCap += probDemandExceeds * f[i][q * capacity - k];
			probDemandExceeds = 0;
		}
	}

	return probExceedsCap;
}

/*
returnCost: Calcula o custo de ir ao depósito a partir do cliente i e retornar ao cliente j.
i e j são as posições dos clientes na rota orderInRoute;
*/
double returnCost(int i, int j, Graph g, vector<int> orderInRoute) {
	return g.adjMatrix[orderInRoute[i]][0] + g.adjMatrix[0][orderInRoute[j]] - g.adjMatrix[orderInRoute[i]][orderInRoute[j]];
}

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
double routeExpectedLength(Graph g, vector<vector<double>> f, int capacity, vector<int> orderInRoute) {

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

/*
totalExpectedLength: Calcula e acumula o custo esperado de todas as rotas.

Saída: double indicando o custo esperado de se percorrer todas as rotas.
*/

double totalExpectedLength(Graph g, int capacity, vector<vector<int>> routes) {

	double totalExpLength = 0;

	for (int r = 0; r < routes.size(); r++) {

		if(routes[r].size() == 0)
			continue;
		vector<vector<double>> f = probTotalDemand(g, routes[r]);
		double routeExpLength = routeExpectedLength(g, f, capacity, routes[r]);

		if(false) {//verbosity == 'y') {
			cout << "------------------------------------------------------------\n\n";
			cout << "f(i,j): prob. of total demand being equal to j on the ith client of the route\n\n";
			for (int i = 1; i <= routes[r].size(); i++) {
	        	for (int j = 0; j <= 20*routes[r].size(); j++) {
	        		cout << "f("<< i << "," << j << "): ";
	            	cout << f[i][j] << " ";
	        	}
	        	cout << "\n\n";
	    	}
			cout << "------------------------------------------------------------\n\n";
		}

		//cout << "Expected length of route " << r + 1 << ": ";
		//cout << routeExpLength << endl;

		totalExpLength += routeExpLength;

	}

	return totalExpLength;

}

/*
randomRoutes: Algoritmo que gera m rotas aleatórias para testar as funções de
segundo estágio, tal que m é o número de veículos.

Saída:
routes: vetor de rotas, cada uma um vetor de inteiros indicando a ordem em que
os vértices devem ser percorridos.
*/
vector<vector<int>> randomRoutes(int numberVertices, int numberVehicles) {

    vector<int> costumers;

    for (int i = 1; i < numberVertices; i++)
        costumers.push_back(i);

    srand(time(0));
    random_shuffle(costumers.begin(), costumers.end(), [](int i){ return rand()%i; });

    vector<vector<int>> routes(numberVehicles);
    int routeSize = numberVertices / numberVehicles;

    cout << endl;

    for (int r = 0; r < numberVehicles; r++) {

        auto start_it = next(costumers.begin(), r*routeSize);
        auto end_it = next(costumers.begin(), r*routeSize + routeSize);

        routes[r].resize(routeSize);

        // Colocar o que restou na última rota
        if (r == numberVehicles - 1) {
            end_it = costumers.end();
            routes[r].resize(costumers.size() - r*routeSize);
        }

        copy(start_it, end_it, routes[r].begin());

        cout << "Route " << r + 1 << ": ";

        for (int i = 0; i < routes[r].size(); i++)
            cout << routes[r][i] << ' ';

        cout << endl;

    }

    cout << endl;

    return routes;

}

double costCen(vector<vector<int>> A, vector<int> B, Graph g, vector<int> route, int capacity) {

  int iter = 0, currCap = 0, currClient = 0, proxClient = route[0];
	double costRoute = 0;

	while(iter != route.size()-1) {

		costRoute += g.adjMatrix[currClient][proxClient];

		if(currCap + A[iter][B[iter]] == capacity) {

			costRoute += g.adjMatrix[proxClient][0];
			currCap = 0;
			currClient = 0;
			iter++;
			proxClient = route[iter];
		}

		else if(currCap + A[iter][B[iter]] > capacity) {

			costRoute += g.adjMatrix[proxClient][0];
			currCap = currCap - capacity;
			currClient = 0;
		}

		else {

			currCap += A[iter][B[iter]];
			iter++;
			currClient = proxClient;
			proxClient = route[iter];
		}

	}

	costRoute += g.adjMatrix[currClient][proxClient];

	if(currCap + A[iter][B[iter]] > capacity) {

		costRoute += g.adjMatrix[proxClient][0];
		costRoute += g.adjMatrix[0][proxClient];
		costRoute += g.adjMatrix[proxClient][0];
	}

	else {

		costRoute += g.adjMatrix[proxClient][0];
	}

	return costRoute;
}

double bruteForce(Graph g, int capacity, vector<vector<int>> routes) {

	double expectedRoutesCost = 0, acc, acc1;
	vector<int> presence, cenRoute;
	int iter = 0;

	for(int i = 0; i < routes.size(); i++) {

		presence.resize(routes[i].size());
		presence[0] = 1;
		for(int j = 1; j < routes[i].size(); j++) {
			presence[j] = 0;
		}

		acc1 = 0;
		for(int k = 0; k < pow(2,routes[i].size())-1; k++) {

			for(int l = 0; l < routes[i].size(); l++) {
				if(presence[l] == 1)
					cenRoute.push_back(routes[i][l]);
			}

			acc = bruteForceCost(g, capacity, cenRoute);
			for(int j = 0; j < routes[i].size(); j++) {
				if(presence[j] == 0) {
					acc *= (1 - g.vertices[routes[i][j]].probOfPresence);
				}
				else {
					acc *= g.vertices[routes[i][j]].probOfPresence;
				}
			}

			cenRoute.clear();
			iter = 0;
			while(iter < routes[i].size()) {
				if(presence[iter] == 0) {
					presence[iter] = 1;
					break;
				}
				presence[iter] = 0;
				iter++;
			}
			acc1 += acc;
		}
		expectedRoutesCost += acc1;
	}

	return expectedRoutesCost;

}

double bruteForceCost(Graph g, int capacity, vector<int> route) {

	if(route.empty())
		return 0;

	double expectedRouteCost = 0, expectedCenCost = 0;
	int numClients = route.size(), numDiffDemand = 11, i, j, k;

	vector<int> col(numDiffDemand+1, -1);
	vector<vector<int>> A(numClients, col);

	for(i = 0; i < numClients; i++) {

		k = 0;
		while(g.vertices[route[i]].probDemand[k] == 0) k++;

		for(j = 0; j < numDiffDemand; j++) {

			if(g.vertices[route[i]].probDemand[k] != 0)
				A[i][j] = k;
			else
				break;

			k++;
		}
	}

	vector<int> B(numClients, 0);

	for(i = 0; i < pow(numDiffDemand,numClients); i++) {

		double acc = costCen(A, B, g, route, capacity);

		for(int k = 0; k < numClients; k++) {
			acc *= g.vertices[route[k]].probDemand[A[k][B[k]]];
		}

		expectedRouteCost += acc;

		for(j = 0; j < numClients; j++) {

			if(A[j][B[j]+1] != -1) {

				for(k = j-1; k > -1; k--)
					B[k] = 0;

				B[j]++;
				break;
			}
		}

		if(j == numClients)
			break;

	}

	return expectedRouteCost;
}

void drawRoutes(Graph g, svrpSol solution, string nameOutputFile) {

	vector<int> l(g.numberVertices, -1);
	vector<vector<int>> routeMatrix(g.numberVertices, l);

	int routeColor = 0;

	for(int i = 0; i < solution.routes.size(); i++) {

		if(solution.routes[i].size() > 0) {

			routeMatrix[0][solution.routes[i][0]] = routeColor;
			routeMatrix[solution.routes[i][0]][0] = routeColor;

			int j;
			for(j = 0; j < solution.routes[i].size()-1; j++) {
				routeMatrix[solution.routes[i][j]][solution.routes[i][j+1]] = routeColor;
				routeMatrix[solution.routes[i][j+1]][solution.routes[i][j]] = routeColor;
			}

			routeMatrix[solution.routes[i][j]][0] = routeColor;
			routeMatrix[0][solution.routes[i][j]] = routeColor;

			routeColor++;
		}
	}

	for(int i = 0; i < g.numberVertices; i++) {

		for(int j = i+1; j < g.numberVertices; j++) {

			g.adjMatrix[i][j] = routeMatrix[i][j];
			g.adjMatrix[j][i] = routeMatrix[j][i];

		}
	}

	g.drawGraph(nameOutputFile);

}
