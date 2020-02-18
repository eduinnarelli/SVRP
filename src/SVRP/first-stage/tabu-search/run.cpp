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
		this->bestFeasibleSol.expectedCost = 
            Bertsimas2ndStage::totalExpectedLength(inst, capacity, this->bestFeasibleSol.routes);

	}

	return this->bestFeasibleSol;
}
