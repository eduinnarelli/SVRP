#include <iostream>
#include <algorithm>
#include <cmath>
#include <time.h>
#include "SVRP_1st_stage.h"

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