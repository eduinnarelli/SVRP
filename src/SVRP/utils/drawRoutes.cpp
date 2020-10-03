#include "SVRP.h"

void drawRoutes(Graph g, vector<vector<int> > solution, string nameOutputFile)
{

    vector<int> l(g.numberVertices, -1);
    vector<vector<int> > routeMatrix(g.numberVertices, l);

    int routeColor = 0;

    for (int i = 0; i < solution.size(); i++) {

        if (solution[i].size() > 0) {

            routeMatrix[0][solution[i][0]] = routeColor;
            routeMatrix[solution[i][0]][0] = routeColor;

            int j;
            for (j = 0; j < solution[i].size() - 1; j++) {
                routeMatrix[solution[i][j]][solution[i][j + 1]] = routeColor;
                routeMatrix[solution[i][j + 1]][solution[i][j]] = routeColor;
            }

            routeMatrix[solution[i][j]][0] = routeColor;
            routeMatrix[0][solution[i][j]] = routeColor;

            routeColor++;
        }
    }

    for (int i = 0; i < g.numberVertices; i++) {

        for (int j = i + 1; j < g.numberVertices; j++) {

            g.adjMatrix[i][j] = routeMatrix[i][j];
            g.adjMatrix[j][i] = routeMatrix[j][i];
        }
    }

    g.drawGraph(nameOutputFile);
}
