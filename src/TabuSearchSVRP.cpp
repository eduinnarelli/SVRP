#include "SVRP.h"

// Fluxo de execução da busca tabu
svrpSol TabuSearchSVRP::run(Graph inst, int numVehicles, int capacity) {

	if(inst.numberVertices > 2) {

		initialize(inst, numVehicles, capacity);
		int i;
		char next = 'n';

		for (i = 0; i < MAX_ITERATIONS; i++) {

			if(verbosity == 'y'){
				cout << "ITERACAO " << i << endl;
				cout << "penalty = " << penalty << endl;
			}

			neighbourhoodSearch();
			update();

			// Intensificar ou terminar
			if(this->currNoImprovement >= this->maxNoImprovement) {

				if(verbosity == 'y')
					cout << "INTENSIFY" << endl;

				if (this->maxNoImprovement == 50*inst.numberVertices) {
					this->numNearest = min(inst.numberVertices - 1, 10);
					this->numSelected = inst.numberVertices - 1;
					this->currNoImprovement = 0;
					this->maxNoImprovement = 100;

					if(!this->bestFeasibleSol.routes.empty()) {

						if(verbosity == 'y')
							cout << "Alguma solucao viavel foi encontrada com custo: " << this->bestFeasibleSol.expectedCost << endl;

						this->sol = this->bestFeasibleSol;

						/* Recuperar rota dos clientes e numero de rotas */
						this->numRoutes = 0;
						for(int j = 0; j < this->sol.routes.size(); j++) {
							for(int k = 0; k < this->sol.routes[j].size(); k++) {
								routeOfClient[this->sol.routes[j][k]] = j;
							}
							this->numRoutes++;
						}
					}

					else {

						if(verbosity == 'y')
							cout << "Nenhuma solucao viavel encontrada" << endl;

					}

				}

				else
					break;

			}

		}

	}

	else {

		this->bestFeasibleSol.routes.clear();
		vector<int> route(1, 1);
		this->bestFeasibleSol.routes.push_back(route);
		this->bestFeasibleSol.expectedCost = totalExpectedLength(inst, capacity, this->bestFeasibleSol.routes);

	}

	return this->bestFeasibleSol;
}

/* Etapa 1: construir solução e estruturas iniciais */
void TabuSearchSVRP::initialize(Graph inst, int numVehicles, int capacity) {

	if(verbosity == 'y')
		cout << endl << "INITIALIZE" << endl;

	this->g = inst;
  this->numVehicles = numVehicles;
  this->capacity = capacity;

  int h = min(this->g.numberVertices - 1, 10);

	this->closestNeighbours.clear();

  // Computar os h vizinhos mais próximos de cada vértice
  for (int i = 0; i < this->g.numberVertices; i++) {

    vector<int> closest(this->g.numberVertices-1); // -1, pois não pegamos o depósito
    vector<double> dist = this->g.adjMatrix[i];

    iota(closest.begin(), closest.end(), 1); // Com 0 pegamos o depósito

    sort(closest.begin(), closest.end(), [&dist](size_t i1, size_t i2){
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
    this->routeOfClient[i] = i-1;

  }

  this->penalty = 1;
	this->numRoutes = this->g.numberVertices - 1;

  this->sol.expectedCost = penalizedExpectedLength(this->sol.routes);
	this->bestPenalExpCost = this->sol.expectedCost;

	if(verbosity == 'y')
		cout << "Demandas relativas:" << endl;

  // Coeficiente que relaciona a demanda esperada do vértice com a total
  for (int i = 1; i < this->g.numberVertices; i++) {

    relativeDemand.push_back(this->g.expectedDemand[i] / this->g.totalExpectedDemand);

		if(verbosity == 'y')
			cout << relativeDemand[i-1] << endl;

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
			if(numVehicles == this->g.numberVertices - 1) {
      	this->bestFeasibleSol.expectedCost = this->sol.expectedCost;
      	this->bestFeasibleSol.routes = this->sol.routes;
			}
			else {
				this->bestFeasibleSol.routes.clear();
				this->bestFeasibleSol.expectedCost = numeric_limits<double>::max();
			}
  }

  this->maxNoImprovement = 50*this->g.numberVertices;

	if(verbosity == 'y')
		cout << "END_INITIALIZE" << endl << endl;
}


/* Etapa 2: avaliar soluções a partir de movimentos entre vizinhos */
void TabuSearchSVRP::neighbourhoodSearch() {

	if(verbosity == 'y')
		cout << "NEIGHBOURHOOD_SEARCH" << endl;

	vector<routeMove> bestMoves;

  this->itCount++;
  this->currNoImprovement++;

	vector<int> customers;
	for(int i = 0; i < this->g.numberVertices - 1; i++) {
		customers.push_back(i+1);
	}

	// Considerar todos movimentos candidatos na vizinhança
	for(int i = 0; i < this->numSelected; i++) {

		routeMove newMove;

		newMove.client = customers[rand()%(this->g.numberVertices - 1)];
		newMove.valid = true;
		newMove.clientRoute = routeOfClient[newMove.client];

		//Escolher um vizinho desse cliente aleatoriamente
		int neighbourIdx = rand()%this->numNearest;
		newMove.neighbour = this->closestNeighbours[newMove.client][neighbourIdx];

		/* Computar o custo aproximado de se remover o cliente de sua rota e
    	inserí-lo imediatamente ou antes do vizinho escolhido. */

		newMove.approxCost = approxMoveCost(newMove);

		if(verbosity == 'y'){
			cout << "Movimento " << i << ": " << newMove.client << " " << newMove.neighbour << endl;
			cout << "Custo: " << newMove.approxCost << endl;
		}

		// Inserir na lista de movimentos em ordem não-decrescente
		auto pos = find_if(bestMoves.begin(), bestMoves.end(), [newMove](routeMove m){
			return m.approxCost > newMove.approxCost;
		});

		bestMoves.insert(pos, newMove);
  }

	if(verbosity == 'y')
		cout << "bestMoves: " << endl;

	for(int i = 0; i < bestMoves.size(); i++) {

		if(verbosity == 'y')
			cout << bestMoves[i].approxCost << " ";

	}

	if(verbosity == 'y')
		cout << endl;

	/* Analisar os 5 primeiros movimentos. Se algum deles for melhor que a solução
	atual, mesmo que seja um movimento tabu (por causa do critério de aspiração),
	então o menor destes movimentos será realizado. Se não, o melhor movimento
	não tabu é realizado, analisando em blocos de 5 */
	int numTabuMoves = 0;
	routeMove bestMoveNotTabu;
	double bestMovePenalExpCost = numeric_limits<double>::max(), bestMoveNotTabuPenalExpCost = numeric_limits<double>::max();
	vector<vector<int>> bestRoutes, bestNotTabuRoutes;
	this->moveDone.valid = false;
	bestMoveNotTabu.valid = false;

	for(int i = 0; i < min(5,(int)bestMoves.size()); i++) {

		routeMove currMove = bestMoves[i];

		if(verbosity == 'y')
			cout << "Analisando movimento " << i << ": " << currMove.client << " " << currMove.neighbour << endl;

		// Checar se é um movimento tabu
		auto tabuPos = find_if(tabuMoves.begin(), tabuMoves.end(), [currMove](routeMove m){
			return m == currMove;
		});

		bool notTabu = (tabuPos == tabuMoves.end());

		vector<vector<int>> actualSol = this->sol.routes;
		double movePenalExpCost;

		if(routeOfClient[currMove.client] == routeOfClient[currMove.neighbour]) {

				vector<int> sameRoute = actualSol[routeOfClient[currMove.client]];

				if(verbosity == 'y')
					cout << "Rota de ambos antes da troca: ";

				for(int j = 0; j < sameRoute.size(); j++) {

					if(verbosity == 'y')
						cout << sameRoute[j] << " ";

				}

				if(verbosity == 'y')
					cout << endl;

				vector<int>::iterator clientPos, neighbourPos;
				for(int j = 0; j < sameRoute.size(); j++) {
					if(sameRoute[j] == currMove.client) {
						clientPos = sameRoute.begin() + j;
					}
					else if(sameRoute[j] == currMove.neighbour) {
						neighbourPos = sameRoute.begin() + j;
					}
				}

				if(clientPos < neighbourPos) {
					sameRoute.erase(remove(sameRoute.begin(), sameRoute.end(), currMove.client));
					sameRoute.insert(neighbourPos-1, currMove.client);
				}
				else {
					sameRoute.erase(remove(sameRoute.begin(), sameRoute.end(), currMove.client));
					sameRoute.insert(neighbourPos, currMove.client);
				}

				if(verbosity == 'y')
					cout << "Rota de ambos apos a troca: ";

				for(int j = 0; j < sameRoute.size(); j++) {

					if(verbosity == 'y')
						cout << sameRoute[j] << " ";

				}

				if(verbosity == 'y')
					cout << endl;

				actualSol[routeOfClient[currMove.client]] = sameRoute;

				// Computar custo esperado e armazenar a melhor solução encontrada
				movePenalExpCost = penalizedExpectedLength(actualSol);
		}

		else {

			vector<int> clientRoute = actualSol[routeOfClient[currMove.client]];
			vector<int> neighbourRoute = actualSol[routeOfClient[currMove.neighbour]];

			int neighbour = currMove.neighbour;
			auto neighbourPos = find_if(neighbourRoute.begin(), neighbourRoute.end(), [neighbour](int x){
				return x == neighbour;
			});

			if(verbosity == 'y')
				cout << "Rota do vizinho antes de ser adicionado: ";

			for(int j = 0; j < neighbourRoute.size(); j++) {

				if(verbosity == 'y')
					cout << neighbourRoute[j] << " ";

			}

			if(verbosity == 'y')
				cout << endl;

			neighbourRoute.insert(neighbourPos, currMove.client);

			if(verbosity == 'y')
				cout << "Rota do vizinho apos ser adicionado: ";

			for(int j = 0; j < neighbourRoute.size(); j++) {

				if(verbosity == 'y')
					cout << neighbourRoute[j] << " ";

			}

			if(verbosity == 'y'){cout << endl;}

			if(verbosity == 'y')
				cout << "Rota do cliente antes de ser removido: ";

			for(int j = 0; j < clientRoute.size(); j++) {

				if(verbosity == 'y')
					cout << clientRoute[j] << " ";

			}

			if(verbosity == 'y')
				cout << endl;

			// Remover cliente da sua rota e inserir na rota vizinha
			clientRoute.erase(remove(clientRoute.begin(), clientRoute.end(), currMove.client));

			if(verbosity == 'y')
				cout << "Rota do cliente apos ser removido: ";

			for(int j = 0; j < clientRoute.size(); j++) {

				if(verbosity == 'y')
					cout << clientRoute[j] << " ";

			}

			if(verbosity == 'y')
				cout << endl;

			if (clientRoute.empty())
				this->numRoutes--;

			actualSol[routeOfClient[currMove.client]] = clientRoute;
			actualSol[routeOfClient[currMove.neighbour]] = neighbourRoute;

			// Computar custo esperado e armazenar a melhor solução encontrada
			movePenalExpCost = penalizedExpectedLength(actualSol);

			if (clientRoute.empty())
				this->numRoutes++;
		}

		if (movePenalExpCost < bestMovePenalExpCost) {
			bestMovePenalExpCost = movePenalExpCost;
			bestRoutes = actualSol;
			this->moveDone = currMove;
		}

		if(notTabu) {
			if(movePenalExpCost < bestMoveNotTabuPenalExpCost) {
				bestMoveNotTabuPenalExpCost = movePenalExpCost;
				bestNotTabuRoutes = actualSol;
				bestMoveNotTabu = currMove;
			}
		}
	}

	/* Possível critério de aspiração */
	if(bestMovePenalExpCost < sol.expectedCost) {
		sol.expectedCost = bestMovePenalExpCost;
		sol.routes = bestRoutes;
		return;
	}

	else if(numTabuMoves == 5) {

		numTabuMoves = 0;
		bestMovePenalExpCost = numeric_limits<double>::max();
		this->moveDone.valid = false;
		bestMoveNotTabu.valid = false;

		for(int i = 5; i < bestMoves.size(); i++) {

			if(i > 5 && i % 5 == 0 && numTabuMoves != 5)
				break;
			else if(i % 5 == 0 && numTabuMoves == 5)
				numTabuMoves = 0;

			routeMove currMove = bestMoves[i];

			if(verbosity == 'y')
				cout << "Analisando movimento " << i << ": " << currMove.client << " " << currMove.neighbour << endl;

			// Checar se é um movimento tabu
			auto tabuPos = find_if(tabuMoves.begin(), tabuMoves.end(), [currMove](routeMove m){
				return m == currMove;
			});

			if(tabuPos != tabuMoves.end()) {
				numTabuMoves++;
				continue;
			}

			vector<vector<int>> actualSol = this->sol.routes;

			vector<int> clientRoute = actualSol[routeOfClient[currMove.client]];
			vector<int> neighbourRoute = actualSol[routeOfClient[currMove.neighbour]];

			int neighbour = currMove.neighbour;
			auto neighbourPos = find_if(neighbourRoute.begin(), neighbourRoute.end(), [neighbour](int x){
				return x == neighbour;
			});

			if(verbosity == 'y')
				cout << "Rota do vizinho antes de ser adicionado: ";

			for(int j = 0; j < neighbourRoute.size(); j++) {

				if(verbosity == 'y')
					cout << neighbourRoute[j] << " ";

			}

			if(verbosity == 'y')
				cout << endl;

			neighbourRoute.insert(neighbourPos, currMove.client);

			if(verbosity == 'y')
				cout << "Rota do vizinho apos ser adicionado: ";

			for(int j = 0; j < neighbourRoute.size(); j++) {

				if(verbosity == 'y')
					cout << neighbourRoute[j] << " ";

			}

			if(verbosity == 'y')
				cout << endl;

			if(verbosity == 'y')
				cout << "Rota do cliente antes de ser removido: ";

			for(int j = 0; j < clientRoute.size(); j++) {

				if(verbosity == 'y')
					cout << clientRoute[j] << " ";

			}

			if(verbosity == 'y')
				cout << endl;

			// Remover cliente da sua rota e inserir na rota vizinha
			clientRoute.erase(remove(clientRoute.begin(), clientRoute.end(), currMove.client));

			if(verbosity == 'y')
				cout << "Rota do cliente apos ser removido: ";

			for(int j = 0; j < clientRoute.size(); j++) {

				if(verbosity == 'y')
					cout << clientRoute[j] << " ";

			}

			if(verbosity == 'y')
				cout << endl;

			if (clientRoute.empty())
				this->numRoutes--;

			actualSol[routeOfClient[currMove.client]] = clientRoute;
			actualSol[routeOfClient[currMove.neighbour]] = neighbourRoute;

			// Computar custo esperado e armazenar a melhor solução encontrada
			double movePenalExpCost = penalizedExpectedLength(actualSol);

			if (clientRoute.empty())
				this->numRoutes++;

			if (movePenalExpCost < bestMoveNotTabuPenalExpCost) {
				bestMoveNotTabuPenalExpCost = movePenalExpCost;
				bestNotTabuRoutes = actualSol;
				bestMoveNotTabu = currMove;
			}
		}
	}

	this->moveDone = bestMoveNotTabu;
	if(this->moveDone.valid) {
		sol.expectedCost = bestMoveNotTabuPenalExpCost;
		sol.routes = bestNotTabuRoutes;
	}

}

/* Etapas 3 e 4: Atualizar estruturas, parâmetros e melhor solução conforme
a viabilidade da solução corrente. */
void TabuSearchSVRP::update() {

	if(verbosity == 'y')
		cout << "UPDATE" << endl;

	if(this->moveDone.valid) {

		this->numRoutes = 0;
		for(int i = 0; i < this->sol.routes.size(); i++) {
			if(!this->sol.routes[i].empty())
				this->numRoutes++;
		}

		if(verbosity == 'y')
			cout << "Movimento escolhido: " << this->moveDone.client << " " << this->moveDone.neighbour << endl;

		if(verbosity == 'y')
			cout << "Numero de rotas atual: " << this->numRoutes << endl;

		// Checar se ao menos melhorou
		if (this->sol.expectedCost < this->bestPenalExpCost) {

			if(verbosity == 'y')
				cout << "Solucao melhorou (F(x) < F*)" << endl;

			this->bestPenalExpCost = this->sol.expectedCost;
			this->currNoImprovement = 0;
		}

		// Inviável
		if (this->numRoutes != this->numVehicles) {

			if(verbosity == 'y')
				cout << "Solucao inviavel" << endl;

			this->numInfeasibleNearby += 1;

		// Viável
		} else {
			this->numInfeasibleNearby = 0;

			if(verbosity == 'y')
				cout << "Solucao viavel" << endl;

			if (this->sol.expectedCost < this->bestFeasibleSol.expectedCost) {

				if(verbosity == 'y')
					cout << "Solucao viavel melhorou (F(x)=T(x) < T*)" << endl;

				this->bestFeasibleSol.expectedCost = this->sol.expectedCost;
				this->bestFeasibleSol = this->sol;
			}

		}

		routeOfClient[moveDone.client] = routeOfClient[moveDone.neighbour];
		if(this->g.numberVertices > 5) {
			this->moveDone.tabuDuration = this->itCount + (this->g.numberVertices - 5) + (rand() % 6);
		}
		else {
			this->moveDone.tabuDuration = this->itCount + (1) + (rand() % 6);
		}

		// Inserir movimento realizado
		this->tabuMoves.push_back(this->moveDone);
	}

	else if(verbosity == 'y')
		cout << "Todos os movimentos sao tabu e nenhum melhora" << endl;

	for (int i = 0; i < this->tabuMoves.size(); i++) {

		// Remover da lista de movimentos tabus se ultrapassou duração
		if (this->itCount  >= tabuMoves[i].tabuDuration) {

			routeMove toBeErased = this->tabuMoves[i];

			this->tabuMoves.erase(remove_if(this->tabuMoves.begin(), this->tabuMoves.end(), [toBeErased](routeMove m){
				return m == toBeErased;
			}), this->tabuMoves.end());

		}

	}

	// Atualizar penalidade
	if (this->itCount % 10 == 0) {
		this->penalty *= pow(2, this->numInfeasibleNearby/5 - 1);
		this->numInfeasibleNearby = 0;
	}

}


// Função objetivo com penalização de soluções inviáveis
double TabuSearchSVRP::penalizedExpectedLength(vector<vector<int>> sol) {

		double aux = totalExpectedLength(g, capacity, sol) + penalty*abs(this->numRoutes - numVehicles);

		if(verbosity=='y')
			cout << "Custo penalizado total: " << aux << endl;

    return aux;
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
double TabuSearchSVRP::approxMoveCost(routeMove m) {

	double approxCost = 0;
	int beforeClient = 0, beforeNeighbour = 0, afterClient = 0;

	/*if(sol.routes[routeOfClient[m.client]].size() == 2 && sol.routes[routeOfClient[m.client]][0])
		afterClient = sol.routes[routeOfClient[m.client]][1];*/
	if(sol.routes[routeOfClient[m.client]].size() >= 2 && sol.routes[routeOfClient[m.client]][0] == m.client) {
		afterClient = sol.routes[routeOfClient[m.client]][1];
	}

	else if(sol.routes[routeOfClient[m.client]].size() >= 2) {

		for(int i = 1; i < sol.routes[routeOfClient[m.client]].size(); i++) {

			if(sol.routes[routeOfClient[m.client]][i] == m.client) {

				beforeClient = sol.routes[routeOfClient[m.client]][i-1];

				//if(sol.routes[routeOfClient[m.client]].size() > i)
				if(i < sol.routes[routeOfClient[m.client]].size() - 1)
					afterClient = sol.routes[routeOfClient[m.client]][i+1];
				else
					afterClient = 0;

			}
		}
	}

	for(int i = 1; i < sol.routes[routeOfClient[m.neighbour]].size(); i++) {

		if(sol.routes[routeOfClient[m.neighbour]][i] == m.neighbour)
			beforeNeighbour = sol.routes[routeOfClient[m.neighbour]][i-1];

	}

	if(verbosity == 'y') {
		cout << "Custo Aproximado:" << endl;
		cout << "beforeClient = " << beforeClient << " beforeNeighbour = " << beforeNeighbour << " afterClient = " << afterClient << endl;
	}

	if(routeOfClient[m.client] == routeOfClient[m.neighbour]) {

		approxCost = approxInsertImpact(beforeNeighbour, m.client, m.neighbour)
					- approxInsertImpact(beforeClient, m.client, afterClient);

	} else {

		approxCost = approxInsertImpact(beforeNeighbour, m.client, m.neighbour)
					- approxInsertImpact(beforeClient, m.client, afterClient)
					+ maxRemovalCost(routeOfClient[m.neighbour])*relativeDemand[m.client - 1]
					- removalCost(routeOfClient[m.client], m.client);

	}

	// Se a solução for inviável e numRoutes > numVehicles
	if(numRoutes > numVehicles) {
		approxCost += penalty*(1/(double)(sol.routes[routeOfClient[m.neighbour]].size()+1)
													-1/(double)(sol.routes[routeOfClient[m.client]].size()));
	}

	// Se a solução for viável
	else {
		if(sol.routes[routeOfClient[m.client]].size() == 1) {
			approxCost += penalty*abs(numRoutes - 1 - numVehicles) - penalty*abs(numRoutes - numVehicles);
		}
		else {
			approxCost += penalty*abs(numRoutes - numVehicles) - penalty*abs(numRoutes - numVehicles);
		}
	}

	return approxCost;

}
