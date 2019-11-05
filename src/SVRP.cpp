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

	f[0][0] = 1; // termo neutro do produtório

	while (orderInRoute <= routeSize) {

		// Considera o próximo vértice da rota
		next = route[orderInRoute - 1];

		// Para todas as demandas possíveis
		for (int dem = 1; dem <= 20 * routeSize; dem++) {

			// Se for o primeiro vértice da rota, a probabilidade depende apenas da sua própria demanda
			if (orderInRoute == 1 && dem <= 20) {

				f[1][dem] = g.vertices[next].probDemand[dem];

			// Se não, consideramos as probabilidades de todas as demandas possíveis dos vértices anteriores
			} else if (dem <= 20 * orderInRoute) {

				for (int k = 1; k <= min(20, dem); k++) {

					double probDemandK = g.vertices[next].probDemand[k];

					if (probDemandK != 0) {
						f[orderInRoute][dem] += probDemandK * f[orderInRoute - 1][dem - k];
					}
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
			probReachCap += g.vertices[vtx].probDemand[k] * f[i][q * capacity - k];
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
				probDemandExceeds += g.vertices[vtx].probDemand[r];
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

	double expectedLength = 0, accPresence = 1, probReachCap;
	int sizeRoute = orderInRoute.size();

	/* Como um vértice pode não estar presente, computa-se o custo da possibilidade de cada
	vértice ser o primeiro,	o que é igual a ele estar presente e os anteriores não, vezes a
	sua distância ao depósito */
	for (int i = 0; i < sizeRoute; i++) {

		for (int r = 0; r <= i - 1; r++) {
			accPresence *= (1 - g.vertices[orderInRoute[r]].probOfPresence);
		}

		expectedLength += g.adjMatrix[0][orderInRoute[i]] * g.vertices[orderInRoute[i]].probOfPresence * accPresence;
		accPresence = 1;
	}

	/* Como um vértice pode não estar presente, computa-se o custo de cada vértice ser o último,
	o que é igual a ele estar presente e os posteriores não estarem presentes, vezes a distância
	ao depósito */
	for (int i = 0; i < sizeRoute; i++) {

		for (int r = i + 1; r < sizeRoute; r++) {
			accPresence *= (1 - g.vertices[orderInRoute[r]].probOfPresence);
		}

		expectedLength += g.adjMatrix[orderInRoute[i]][0] * g.vertices[orderInRoute[i]].probOfPresence * accPresence;
		accPresence = 1;
	}

	/* Como um vértice pode não estar presente, computa-se o custo de cada possível próximo vértice
	da rota, o que é igual a dois vértices estarem presentes e os vértices entre eles não, vezes
	a distância entre esses vértices */
	for (int i = 0; i < sizeRoute; i++) {

		for (int j = i + 1; j < sizeRoute; j++) {

			for (int r = i + 1; r <= j - 1; r++) {
				accPresence *= (1 - g.vertices[orderInRoute[r]].probOfPresence);
			}

			double probBothPresent = g.vertices[orderInRoute[i]].probOfPresence * g.vertices[orderInRoute[j]].probOfPresence;
			expectedLength += g.adjMatrix[orderInRoute[i]][orderInRoute[j]] * probBothPresent * accPresence;
			accPresence = 1;
		}
	}

	/* Computar o custo esperado da capacidade limite ser atingida e o veículo retornar
	ao depósito. */
	for (int i = 0; i < sizeRoute; i++) {

		// Somar a probabilidade de exceder vezes o custo de retornar ao depósito
		expectedLength += probExceedsCapacity(i, g, f, capacity, orderInRoute) * returnCost(i, i, g, orderInRoute);

		// Salvar probabilidade de atingir exatamente a capacidade no cliente i
		probReachCap = probReachCapacity(i, g, f, capacity, orderInRoute);

		// Somar a probabilidade de atingir exatamente a capacidade vezes o custo de retornar ao depósito
		for (int j = i + 1; j < sizeRoute; j++) {

			for (int r = i + 1; r <= j - 1; r++) {
				accPresence *= (1 - g.vertices[orderInRoute[r]].probOfPresence);
			}

			expectedLength += probReachCap * g.vertices[orderInRoute[j]].probOfPresence * returnCost(i, j, g, orderInRoute) * accPresence;
			accPresence = 1;
		}
	}

	return expectedLength;

}

/*
totalExpectedLength: Calcula e acumula o custo esperado de todas as rotas.

Saída: double indicando o custo esperado de se percorrer todas as rotas.
*/

double totalExpectedLength(Graph g, int capacity, vector<vector<int>> routes, char verbosity) {

	double totalExpLength = 0;

	for (int r = 0; r < routes.size(); r++) {

		vector<vector<double>> f = probTotalDemand(g, routes[r]);
		double routeExpLength = routeExpectedLength(g, f, capacity, routes[r]);

		if(verbosity == 'y') {
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

		cout << "Expected length of route " << r + 1 << ": ";
		cout << routeExpLength << "\n\n";

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