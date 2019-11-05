#include<iomanip>
#include "SVRP.h"

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
	vector<vector<int>> routes = randomRoutes(numberVertices, numberVehicles);

	cout << "Total expected length: ";
	cout << totalExpectedLength(graph, capacity, routes, verbosity) << endl;

    return 0;

}
