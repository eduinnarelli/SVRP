#include "SVRP.h"

double BruteForce2ndStage::routeExpectedLength(Graph g, int capacity, vector<int> route) {

	if(route.empty())
		return 0;

	double expectedRouteCost = 0;
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

		double acc = BruteForce2ndStage::scenaryCost(A, B, g, route, capacity);

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
