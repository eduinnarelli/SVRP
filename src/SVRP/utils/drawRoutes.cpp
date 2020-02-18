#include "SVRP.h"

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
