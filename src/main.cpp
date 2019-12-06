#include <iomanip>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
//#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "SVRP.h"

char verbosity;

int main(int argc, const char **argv) {

  Graph graph;
  double fillingCoeff;
	int capacity, numberVertices, numberVehicles;
  char saveFile, saveEps;
  ifstream instanceFile;
  stringstream input;
  string line;

  if(argc == 2) {
    instanceFile.open(argv[1], std::ios::in | std::ios::binary);

  	if (!instanceFile.is_open()) {
    		printf("ERROR: Not possible to open instance file.\n");
  		return 1;
  	}

    getline(instanceFile, line);
		input = stringstream(line);
		input >> numberVertices;
    getline(instanceFile, line);
		input = stringstream(line);
		input >> numberVehicles;
    getline(instanceFile, line);
    input = stringstream(line);
    input >> fillingCoeff;
    getline(instanceFile, line);
    input = stringstream(line);
    input >> verbosity;
    getline(instanceFile, line);
    input = stringstream(line);
    input >> saveFile;
  }

  else {

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

    do {
      cout << "Visualize best solution in eps file? (y/n): ";
      cin >> saveEps;
    } while(saveEps != 'y' && saveEps != 'n');

  }

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

  string nameOutputFile = "output/";
  nameOutputFile += "BestSolN" + to_string(numberVertices)
                  + "M" + to_string(numberVehicles)
                  + "f" + to_string(fillingCoeff).substr(0,4);

  if(saveFile == 'y') {

    string txtNameOutputFile = nameOutputFile + ".txt";

    ofstream outputFile;

    /* Criar diretorio, se não criado
    int status;
    status = system("mkdir -p output");
    if (status == -1)
      cerr << "Erro ao criar diretorio" << endl;
    */

    outputFile.open(txtNameOutputFile, std::ios::app);

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

  if(saveEps == 'y') {

    nameOutputFile += ".eps";

    if(bestSol.routes.size() != 0)
      drawRoutes(graph, bestSol, nameOutputFile);

  }

  return 0;

}
