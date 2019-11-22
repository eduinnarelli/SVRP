#include <iomanip>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <stdio.h>
#include <io.h>
#include "TabuSearchSVRP.h"

char verbosity;

int main() {

  Graph graph;
  double fillingCoeff;
	int capacity, numberVertices, numberVehicles;
  char saveFile;

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
    cout << "Visualize all problem information in stdio? (y/n): ";
    cin >> verbosity;
  } while(verbosity != 'y' && verbosity != 'n');

  do {
    cout << "Save best solution in txt file? (y/n): ";
    cin >> saveFile;
  } while(saveFile != 'y' && saveFile != 'n');

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

  if(saveFile == 'y') {
    ofstream outputFile;
    string nameOutputFile = "output\\";

    nameOutputFile += "BestSolN" + to_string(numberVertices)
                    + "M" + to_string(numberVehicles)
                    + "f0" + to_string((int)(10*fillingCoeff)) + ".txt";

    mkdir("output");

    outputFile.open(nameOutputFile, std::ios::app);

    if (outputFile.is_open())  {

      if(bestSol.routes.size() == 0)
          outputFile << "Nenhuma solucao viavel encontrada" <<  endl;

      else {

        for(int i = 0; i < bestSol.routes.size(); i++) {
      		outputFile << "Rota " << i << ": ";
      		for(int j = 0; j < bestSol.routes[i].size(); j++) {
      			outputFile << bestSol.routes[i][j] << " ";
      		}
      		outputFile << endl;
      	}
      }

      outputFile << "Custo total: " << bestSol.expectedCost << endl;
      outputFile << "Tempo de processamento: " << elapsed_secs << endl << endl;

      outputFile.close();
    }

    else cout << "Unable to open file";
  }

  else {

    if(bestSol.routes.size() == 0)
        cout << "Nenhuma solucao viavel encontrada" <<  endl;

    else {
      for(int i = 0; i < bestSol.routes.size(); i++) {
        cout << "Rota " << i << ": ";
        for(int j = 0; j < bestSol.routes[i].size(); j++) {
          cout << bestSol.routes[i][j] << " ";
        }
        cout << endl;
      }
    }

    cout << "Custo total: " << bestSol.expectedCost << endl;
    cout << "Tempo de processamento: " << elapsed_secs << endl << endl;

  }

  return 0;

}
