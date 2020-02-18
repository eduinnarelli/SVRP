#include "SVRP.h"

// Custo de remover o cliente r da rota r
double TabuSearchSVRP::removalCost(int r, int client) {

	vector<vector<double>> f = probTotalDemand(g, sol.routes[r]);
	double before = Bertsimas2ndStage::routeExpectedLength(g, f, capacity, sol.routes[r]);

	vector<int> newRoute;
	for(int i = 0; i < sol.routes[r].size(); i++) {
		if(sol.routes[r][i] != client)
			newRoute.push_back(sol.routes[r][i]);
	}

	f = probTotalDemand(g, newRoute);
	double after = Bertsimas2ndStage::routeExpectedLength(g, f, capacity, newRoute);

	return before - after;

}
