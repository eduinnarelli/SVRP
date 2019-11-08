#include <cmath>
#include <algorithm>
#include <numeric>
#include <time.h>
#include "TabuSearchSVRP.h"

// Fluxo de execução da busca tabu
svrpSol TabuSearchSVRP::run(Graph inst, int numVehicles, int capacity) {

	initialize(inst, numVehicles, capacity);

	for (int i = 0; i < MAX_ITERATIONS; i++) {

		neighbourhoodSearch();
		update();

		// Intensificar ou terminar
		if(this->currNoImprovement >= this->maxNoImprovement) {
			
			if (this->maxNoImprovement == 50*inst.numberVertices) {
				this->numNearest = min(inst.numberVertices - 1, 10);
				this->numSelected = inst.numberVertices - 1;
				this->currNoImprovement = 0;
				this->maxNoImprovement = 100;
				this->sol = this->bestFeasibleSol;
			} else {
				break;
			}

		}

	}

	return this->bestFeasibleSol;

}

/* Etapa 1: construir solução e estruturas iniciais */
void TabuSearchSVRP::initialize(Graph inst, int numVehicles, int capacity) {

    this->g = inst;
    this->numVehicles = numVehicles;
    this->capacity = capacity;

    int h = min(this->g.numberVertices - 1, 10);

	srand(time(0));

    // Computar os h vizinhos mais próximos de cada vértice
    for (int i = 0; i < this->g.numberVertices; i++) {

        vector<int> closest(this->g.numberVertices);
        vector<double> dist = this->g.adjMatrix[i];

        iota(closest.begin(), closest.end(), 1);

        sort(closest.begin(), closest.end(), [&dist](size_t i1, size_t i2){
            return dist[i1] < dist[i2];
        });

        closest.erase(closest.begin());
        closest.erase(closest.begin() + h, closest.end());

        this->closestNeighbours.push_back(closest);

    }

    this->routeOfClient.resize(this->g.numberVertices);

    // Rotas de ida e volta ao depósito
    for (int i = 0; i < this->g.numberVertices; i++) {

        vector<int> route(1, i);
        this->sol.routes.push_back(route);
        this->routeOfClient[i] = i;

    }

    this->penalty = 1;
	this->numRoutes = this->g.numberVertices - 1;

    this->sol.expectedCost = penalizedExpectedLength(this->sol.routes);
	this->bestPenalExpCost = this->sol.expectedCost;

    // Coeficiente que relaciona a demanda esperada do vértice com a total
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
        this->bestFeasibleSol.expectedCost = numeric_limits<double>::max();
    } else {
        this->numInfeasibleNearby = 0;
        this->bestFeasibleSol.expectedCost = this->bestPenalExpCost;
        this->bestFeasibleSol.routes = this->sol.routes;
    }

    this->maxNoImprovement = 50*this->g.numberVertices;

}


/* Etapa 2: avaliar soluções a partir de movimentos entre vizinhos */
void TabuSearchSVRP::neighbourhoodSearch() {

	vector<routeMove> bestMoves;

    this->itCount++;
    this->currNoImprovement++;

	srand(time(0));
    vector<int> customers(this->g.numberVertices - 1);

	//Escolher clientes aleatoriamente

	iota(customers.begin(), customers.end(), 1);

    random_shuffle(customers.begin(), customers.end(), [](int i){
    	return rand()%i;
    });

	// Considerar todos movimentos candidatos na vizinhança
	for(int i = 0; i < this->numSelected; i++) {

		routeMove newMove;

		newMove.client = customers[i];
		
		//Escolher um vizinho desse cliente aleatoriamente
		int neighbourIdx = rand()%this->numNearest;
		newMove.neighbour = this->closestNeighbours[newMove.client][neighbourIdx];

		/* Computar o custo aproximado de se remover o cliente de sua rota e
    	inserí-lo imediatamente ou antes do vizinho escolhido. */
		newMove.approxCost = approxMoveCost(newMove, relativeDemand[newMove.client]);

		// Inserir na lista ordenada de movimentos
		auto pos = find_if(bestMoves.begin(), bestMoves.end(), [newMove](routeMove m){
			return m.approxCost < newMove.approxCost;
		});

		newMove.it = this->itCount;

		bestMoves.insert(pos, newMove);

    }

	bool allTabu = true;
	double bestMovePenalExpCost = numeric_limits<double>::max();
	vector<vector<int>> bestRoutes;

	/* Analisar movimentos precisamente em blocos de 5 movimentos. O próximo
	bloco é avaliado se todos movimentos do bloco atual forem tabu. */
	for (int i = 0; i < bestMoves.size(); i++) {

		if (i % 5 == 0 && !allTabu)
			break;

		routeMove currMove = bestMoves[i];

		// Checar se é um movimento tabu

		auto tabuPos = find_if(tabuMoves.begin(), tabuMoves.end(), [currMove](routeMove m){
			return m == currMove;
		});

		bool isTabu = (tabuPos != tabuMoves.end());

		if (!isTabu) {

			vector<vector<int>> actualSol = this->sol.routes;

			vector<int> clientRoute = actualSol[routeOfClient[currMove.client]];
			vector<int> neighbourRoute = actualSol[routeOfClient[currMove.neighbour]];

			// Remover cliente da sua rota e inserir na rota vizinha

			clientRoute.erase(
				remove(clientRoute.begin(), clientRoute.end(), currMove.client),
				clientRoute.end()
			);

			int neighbour = currMove.neighbour;

			auto neighbourPos = find_if(neighbourRoute.begin(), neighbourRoute.end(), [neighbour](int x){
				return x == neighbour;
			});
			
			neighbourRoute.insert(neighbourPos, currMove.client);

			if (clientRoute.empty()) {
				remove(actualSol.begin(), actualSol.end(), clientRoute);
				this->numRoutes--;
			}

			actualSol[routeOfClient[currMove.neighbour]] = neighbourRoute;

			// Computar custo esperado e armazenar a melhor solução encontrada

			double movePenalExpCost = penalizedExpectedLength(actualSol);

			if (movePenalExpCost < bestMovePenalExpCost) {
				bestMovePenalExpCost = movePenalExpCost;
				bestRoutes = actualSol;
				this->moveDone = currMove;
			}

		}

	}

	// Atualizar solução dessa iteração
	this->sol.routes = bestRoutes;
	this->sol.expectedCost = bestMovePenalExpCost;

}

/* Etapas 3 e 4: Atualizar estruturas, parâmetros e melhor solução conforme
a viabilidade da solução corrente. */
void TabuSearchSVRP::update() {

	// Checar se ao menos melhorou
	if (this->sol.expectedCost < this->bestPenalExpCost) {
		this->bestPenalExpCost = this->sol.expectedCost;
		this->currNoImprovement = 0;
	}

	// Inviável
	if (this->numVehicles < this->numRoutes) {
		this->numInfeasibleNearby += 1;

	// Viável
	} else {

		if (this->sol.expectedCost < this->bestFeasibleSol.expectedCost)
			this->bestFeasibleSol = this->sol;

	}

	srand(time(0));
	int tabuDuration = rand() % this->g.numberVertices + (this->g.numberVertices - 5);

	for (int i = 0; i < this->tabuMoves.size(); i++) {

		// Remover da lista de movimentos tabus se ultrapassou duração
		if (this->itCount - this->tabuMoves[i].it >= tabuDuration) {

			routeMove toBeErased = this->tabuMoves[i];

			this->tabuMoves.erase(remove_if(this->tabuMoves.begin(), this->tabuMoves.end(), [toBeErased](routeMove m){
				return m == toBeErased;
			}), this->tabuMoves.end());

		}

	}

	// Inserir movimento realizado
	this->tabuMoves.push_back(this->moveDone);


	// Atualizar penalidade
	if (this->itCount % 10 == 0) {
		this->penalty *= pow(2, this->numInfeasibleNearby/5 - 1);
		this->numInfeasibleNearby = 0;
	}

}


// Função objetivo com penalização de soluções inviáveis
double TabuSearchSVRP::penalizedExpectedLength(vector<vector<int>> sol) {
    return totalExpectedLength(g, capacity, sol) + penalty*abs(this->numRoutes - numVehicles);
}

// Custo de remover o cliente r da rota r
double TabuSearchSVRP::removalCost(int r, int client) {

	vector<vector<double>> f = probTotalDemand(g, sol.routes[r]);
	double before = routeExpectedLength(g, f, capacity, sol.routes[r]);

	vector<int> newRoute;
	for(int i = 0; i < sol.routes[r].size(); i++) {
		if(sol.routes[r][i] != client)
			newRoute.push_back(sol.routes[r][i]);
	}

	f = probTotalDemand(g, newRoute);
	double after = routeExpectedLength(g, f, capacity, newRoute);

	return before - after;

}

/* Maior custo de remover um cliente da rota r, usado como aproximação
do custo de inserção de um cliente. */
double TabuSearchSVRP::maxRemovalCost(int r) {

	double max = 0, aux = 0;

	for(int i = 0; i < sol.routes[r].size(); i++) {

		aux = removalCost(r, sol.routes[r][i]);

		if(aux > max)
			max = aux;

	}

	return max;

}

// Impacto aproximado de introdução do cliente b na rota
double TabuSearchSVRP::approxInsertImpact(int a, int b, int c) {
	return (g.adjMatrix[a][b]+g.adjMatrix[b][c]-g.adjMatrix[a][c])*g.vertices[b].probOfPresence;
}

/* Custo aproximado do movimento de remover o cliente de uma rota e 
inserí-lo imediatamente antes a um vizinho. */
double TabuSearchSVRP::approxMoveCost(routeMove m, double relativeDemand) {

	double approxCost = 0;
	int beforeClient = 0, beforeNeighbour = 0, afterClient = 0;

	if(sol.routes[routeOfClient[m.client]].size() == 2 && sol.routes[routeOfClient[m.client]][0])
		afterClient = sol.routes[routeOfClient[m.client]][1];

	for(int i = 1; i < sol.routes[routeOfClient[m.client]].size(); i++) {

		if(sol.routes[routeOfClient[m.client]][i] == m.client) {

			beforeClient = sol.routes[routeOfClient[m.client]][i-1];

			if(sol.routes[routeOfClient[m.client]].size() > i)
				afterClient = sol.routes[routeOfClient[m.client]][i+1];
			else
				afterClient = 0;

		}

	}

	for(int i = 1; i < sol.routes[routeOfClient[m.neighbour]].size(); i++) {

		if(sol.routes[routeOfClient[m.neighbour]][i] == m.neighbour)
			beforeNeighbour = sol.routes[routeOfClient[m.neighbour]][i-1];

	}


	if(routeOfClient[m.client] == routeOfClient[m.neighbour]) {

		approxCost = approxInsertImpact(beforeNeighbour, m.client, m.neighbour)
					- approxInsertImpact(beforeClient, m.client, afterClient);

	} else {

		approxCost = approxInsertImpact(beforeNeighbour, m.client, m.neighbour)
					- approxInsertImpact(beforeClient, m.client, afterClient)
					+ maxRemovalCost(routeOfClient[m.neighbour])*relativeDemand
					- removalCost(routeOfClient[m.client], m.client);

	}

	approxCost += penalty*(1/(double)(sol.routes[routeOfClient[m.neighbour]].size()+1)
					-1/(double)(sol.routes[routeOfClient[m.client]].size()));

	return approxCost;

}
