#include "SVRP.h"

/* Custo aproximado do movimento de remover o cliente de uma rota e
inserí-lo imediatamente antes a um vizinho. */
double TabuSearchSVRP::approxMoveCost(routeMove m) {

	double approxCost = 0;
	int beforeClient = 0, beforeNeighbour = 0, afterClient = 0;

	if(sol.routes[routeOfClient[m.client]].size() >= 2 && sol.routes[routeOfClient[m.client]][0] == m.client) {
		afterClient = sol.routes[routeOfClient[m.client]][1];
	}

	else if(sol.routes[routeOfClient[m.client]].size() >= 2) {

		for(int i = 1; i < sol.routes[routeOfClient[m.client]].size(); i++) {

			if(sol.routes[routeOfClient[m.client]][i] == m.client) {

				beforeClient = sol.routes[routeOfClient[m.client]][i-1];

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
