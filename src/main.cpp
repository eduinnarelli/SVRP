#include<iomanip>
#include "SVRP.h"

int main() {

    int numberVertices;
    Graph graph;
    int capacity;
    char verbosity;

	/* Número de vértices do problema, incluindo o depósito */
	cout << "Enter with number of vertices including depot: ";
    cin >> numberVertices;

	/* Imprimir todas as informações do grafo ou apenas a resposta */
	cout << "Visualize all problem information? (y/n): ";
	cin >> verbosity;

	// Temporario: capacidade igual a esperança da demanda vezes metade do número de vértices
	capacity = max(20, 10*((numberVertices-1)/2));
	cout << "Capacity: " << capacity << endl;

	/* Cria um grafo completo respeitando a desigualdade triangular */
    graph.createInstance(numberVertices);
    if(verbosity == 'y')
    	graph.printInstance();

    /* Encontra um primeiro caminho utilizando TSP força bruta */
    vector<int> route = graph.TSP();

	/* Dado este caminho, computa todas as probabilidades de demanda total até cada vértice */
    vector<vector<double> > f = probDemandsInRoute(graph, route);

	if(verbosity == 'y') {
		for (int i = 0; i < graph.numberVertices; i++) {
	        for (int j = 0; j <= 20*(graph.numberVertices-1); j++) {
	        	cout << "("<< i << "," << j << "): ";
	            cout << f[i][j] << " ";
	        }
	        cout << "\n\n";
	    }
	}

	cout << "Expected cost of TSP route: ";
	cout << expectedLength(graph, f, capacity, route) << endl;

    return 0;

}
