#include "SVRP.h"

/*
randomRoutes: Algoritmo que gera m rotas aleatórias para testar as funções de
segundo estágio, tal que m é o número de veículos.

Saída:
routes: vetor de rotas, cada uma um vetor de inteiros indicando a ordem em que
os vértices devem ser percorridos.
*/
vector<vector<int>> randomRoutes(int numberVertices, int numberVehicles) {

    vector<int> costumers;

    for (int i = 1; i < numberVertices; i++)
        costumers.push_back(i);

    srand(time(0));
    random_shuffle(costumers.begin(), costumers.end(), [](int i){ return rand()%i; });

    vector<vector<int>> routes(numberVehicles);
    int routeSize = numberVertices / numberVehicles;

    cout << endl;

    for (int r = 0; r < numberVehicles; r++) {

        auto start_it = next(costumers.begin(), r*routeSize);
        auto end_it = next(costumers.begin(), r*routeSize + routeSize);

        routes[r].resize(routeSize);

        // Colocar o que restou na última rota
        if (r == numberVehicles - 1) {
            end_it = costumers.end();
            routes[r].resize(costumers.size() - r*routeSize);
        }

        copy(start_it, end_it, routes[r].begin());

        cout << "Route " << r + 1 << ": ";

        for (int i = 0; i < routes[r].size(); i++)
            cout << routes[r][i] << ' ';

        cout << endl;

    }

    cout << endl;

    return routes;

}
