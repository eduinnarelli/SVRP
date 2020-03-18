#include "SVRP.h"

/*
totalExpectedLength: Calcula e acumula o custo esperado de todas as rotas.

Saída: double indicando o custo esperado de se percorrer todas as rotas.
*/
double Bertsimas2ndStage::totalExpectedLength(Graph g, int capacity, vector<vector<int>> routes) {

	double totalExpLength = 0;

	for (int r = 0; r < routes.size(); r++) {

		if(routes[r].size() == 0)
			continue;
		vector<vector<double>> f = probTotalDemand(g, routes[r]);
		double routeExpLength = Bertsimas2ndStage::routeExpectedLength(g, f, capacity, routes[r]);

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

		totalExpLength += routeExpLength;

	}

	return totalExpLength;

}
