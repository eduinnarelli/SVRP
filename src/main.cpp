#include <iomanip>
#include "TabuSearchSVRP.h"
#include <algorithm>
#include <ctime>

char verbosity;

int main() {

  Graph graph;
  double fillingCoeff;
	int capacity, numberVertices, numberVehicles;

  do {
    cout << "Enter with number of vertices including depot (>1): ";
    cin >> numberVertices;
  } while(numberVertices <= 1);

	do{
    cout << "Enter with number of vehicles (>=1 and less then number of vertices): ";
    cin >> numberVehicles;
  } while(numberVehicles < 1 || numberVehicles > numberVertices-1);

  do {
    cout << "Enter with filling coefficient in interval (0,1]: ";
    cin >> fillingCoeff;
  } while(fillingCoeff <= 0 || fillingCoeff > 1);

  do {
    cout << "Visualize all problem information? (y/n): ";
    cin >> verbosity;
  } while(verbosity != 'y' && verbosity != 'n');

	/* Capacidade regulada de acordo com os dados do problema */
	capacity = max(int(10*(numberVertices-1)/(2*numberVehicles*fillingCoeff)),20);

	if(verbosity == 'y')
    cout << "Capacity of each vehicle: " << capacity << endl;

	/* Criar um grafo completo respeitando a desigualdade triangular */
  graph.createInstance(numberVertices);

  if(verbosity == 'y')
  	graph.printInstance();

	TabuSearchSVRP ts;

  clock_t begin = clock();

	svrpSol bestSol = ts.run(graph, numberVehicles, capacity);

  clock_t end = clock();

  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

  cout << "Melhor solucao viavel encontrada:" << endl;
	for(int i = 0; i < bestSol.routes.size(); i++) {
		cout << "Rota " << i << ": ";
		for(int j = 0; j < bestSol.routes[i].size(); j++) {
			cout << bestSol.routes[i][j] << " ";
		}
		cout << endl;
	}
	cout << "Custo total: " << bestSol.expectedCost << endl;
  cout << "Tempo de processamento: " << elapsed_secs << endl;

  return 0;

}
