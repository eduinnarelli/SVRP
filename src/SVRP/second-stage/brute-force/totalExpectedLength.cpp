#include "SVRP.h"

double BruteForce2ndStage::totalExpectedLength(Graph g, int capacity, vector<vector<int> > routes)
{

    double expectedRoutesCost = 0, acc, acc1;
    vector<int> presence, scenaryRoute;
    int iter = 0;

    for (int i = 0; i < routes.size(); i++) {

        presence.resize(routes[i].size());
        presence[0] = 1;
        for (int j = 1; j < routes[i].size(); j++) {
            presence[j] = 0;
        }

        acc1 = 0;
        for (int k = 0; k < pow(2, routes[i].size()) - 1; k++) {

            for (int l = 0; l < routes[i].size(); l++) {
                if (presence[l] == 1)
                    scenaryRoute.push_back(routes[i][l]);
            }

            acc = BruteForce2ndStage::routeExpectedLength(g, capacity, scenaryRoute);
            for (int j = 0; j < routes[i].size(); j++) {
                if (presence[j] == 0) {
                    acc *= (1 - g.vertices[routes[i][j]].probOfPresence);
                }
                else {
                    acc *= g.vertices[routes[i][j]].probOfPresence;
                }
            }

            scenaryRoute.clear();
            iter = 0;
            while (iter < routes[i].size()) {
                if (presence[iter] == 0) {
                    presence[iter] = 1;
                    break;
                }
                presence[iter] = 0;
                iter++;
            }
            acc1 += acc;
        }
        expectedRoutesCost += acc1;
    }

    return expectedRoutesCost;
}
