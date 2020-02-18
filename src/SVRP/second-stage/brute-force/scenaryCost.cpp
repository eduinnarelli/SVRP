#include "SVRP.h"

double BruteForce2ndStage::scenaryCost(
	vector<vector<int>> A, 
	vector<int> B, 
	Graph g, 
	vector<int> route, 
	int capacity
) {

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
