#include<iomanip>
#include "SVRP_2nd_stage.h"

int main() {

    Graph graph;
    char verbosity;
	double fillingCoeff;
	int capacity, numberVertices, numberVehicles;

	cout << "Enter with number of vertices including depot: ";
    cin >> numberVertices;

	cout << "Enter with number of vehicles: ";
    cin >> numberVehicles;

	cout << "Enter with filling coefficient between 0 and 1: ";
    cin >> fillingCoeff;

	cout << "Visualize all problem information? (y/n): ";
	cin >> verbosity;

	/* Capacidade regulada de acordo com os dados do problema */
	capacity = 10*(numberVertices-1)/(2*numberVehicles*fillingCoeff);
	cout << "\nCapacity of each vehicle: " << capacity << endl;

	/* Criar um grafo completo respeitando a desigualdade triangular */
    graph.createInstance(numberVertices);
    if(verbosity == 'y')
    	graph.printInstance();

    /* Definir rotas do primeiro estágio aleatoriamente */
	double totalExpectedLength = 0;
	vector<vector<int>> routes = randomRoutes(numberVertices, numberVehicles);

	/* Calcular e acumular custo esperado das rotas */
	for (int r = 0; r < numberVehicles; r++) {

		vector<vector<double>> f = probTotalDemand(graph, routes[r]);
		double routeExpectedLength = expectedLength(graph, f, capacity, routes[r]);

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

		cout << "Expected length of route " << r + 1 << ": ";
		cout << routeExpectedLength << "\n\n";

		totalExpectedLength += routeExpectedLength;

	}

	cout << "Total expected length: " << totalExpectedLength << endl;

    return 0;

}
