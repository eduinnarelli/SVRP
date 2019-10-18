#include<iomanip>
#include "SVRP.h"

int main() {

    int numberVertices;
    Graph graph;

    cin >> numberVertices;

    graph.createInstance(numberVertices);
    graph.printInstance();
    vector<int> route = graph.TSP();

    vector<vector<double>> f = probDemandsInRoute(graph, route);

    cout << setprecision(3);

    for (int i = 0; i < graph.numberVertices; i++) {
        for (int j = 0; j < 20*graph.numberVertices; j++) {
            cout << f[i][j] << ' ';
        }
        cout << "\n\n";
    }

    return 0;

}