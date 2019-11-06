#include<iomanip>
#include "TabuSearchSVRP.h"

char verbosity;

int main() {

    Graph graph;
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

	TabuSearchSVRP ts(graph, numberVehicles, capacity);

    /* Definir rotas do primeiro estágio aleatoriamente */
	vector<vector<int>> routes = randomRoutes(numberVertices, numberVehicles);

	cout << "Total expected length: ";
	cout << totalExpectedLength(graph, capacity, routes) << endl;

    return 0;

}
