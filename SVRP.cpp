#include<cmath>
#include "SVRP.h"

vector<vector<double>> probDemandsInRoute(Graph g, vector<int> orderInRoute) {

    vector<double> v(20*g.numberVertices + 1, 0);
    vector<vector<double>> f(g.numberVertices, v);
    int next, m = 1, routeSize = g.numberVertices - 1;

    while (m <= routeSize) {

        next = orderInRoute[m-1];

        for (int r = 1; r <= 20*g.numberVertices; r++) {

            if (m == 1 && r <= 20) {

                f[1][r] = g.vertices[next].probDemand[r];

            } else if (r <= 20*m) {
                
                for (int k = 1; k <= min(20,r); k++) {

                    double probDemandK = g.vertices[next].probDemand[k];

                    if (probDemandK != 0) {
                        f[m][r] += probDemandK*f[m-1][r-k];
                    }

                }

            } 

        }

        m++;

    }

    return f;

}


double probReachCapacity(int i, Graph g, vector<vector<double>> f, int capacity) {

    double probReachCap = 0;

    for (int q = 1; q <= floor(i*20/capacity); q++) {

        for (int k = 1; k <= 20; k++) {
            probReachCap += g.vertices[i].probDemand[k]*f[i-1][q*capacity-k];
        }

    }

    return probReachCap;

}

double probExceedsCapacity(int i, Graph g, vector<vector<double>> f, int capacity) {

    double probExceedsCap = 0, probDemandExceeds = 0;

    for (int q = 1; q <= floor(i*20/capacity); q++) {

        for (int k = 1; k <= 19; k++) {

            for (int r = k + 1; r <= 20; r++) {
                probDemandExceeds += g.vertices[i].probDemand[r];
            }

            probExceedsCap += probDemandExceeds*f[i-1][q*capacity-k];

        }

        probDemandExceeds = 0;

    }

    return probExceedsCap;

}