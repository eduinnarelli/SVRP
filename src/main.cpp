#include <iomanip>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "LShapedSVRP.h"

char verbosity;

int main(int argc, const char** argv)
{

    Graph graph;
    double fillingCoeff;
    int capacity, numberVertices, numberVehicles, numRandomClients;
    char saveFile, saveEps;
    ifstream instanceFile;
    stringstream input;
    string line;

    srand(time(0));

    // Ler entrada de um arquivo
    if (argc == 2) {
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
        input >> numRandomClients;
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
        getline(instanceFile, line);
        input = stringstream(line);
        input >> saveEps;
    }

    // Ler entrada do stdin
    else {

        do {
            cout << "Enter with number of vertices including depot (>1): ";
            cin >> numberVertices;
        } while (numberVertices <= 1);

        do {
            cout << "Enter with number of clients whose presence is uncertain [0,n-1]: ";
            cin >> numRandomClients;
        } while (numRandomClients >= numberVertices);

        do {
            cout << "Enter with number of vehicles (>=1 and less then number of vertices): ";
            cin >> numberVehicles;
        } while (numberVehicles < 1 || numberVehicles > numberVertices - 1);

        do {
            cout << "Enter with filling coefficient in interval (0,1]: ";
            cin >> fillingCoeff;
        } while (fillingCoeff <= 0 || fillingCoeff > 1);

        do {
            cout << "Visualize all problem information in stdio? (y/n): ";
            cin >> verbosity;
        } while (verbosity != 'y' && verbosity != 'n');

        do {
            cout << "Save best solution in txt file? (y/n): ";
            cin >> saveFile;
        } while (saveFile != 'y' && saveFile != 'n');

        do {
            cout << "Visualize best solution in eps file? (y/n): ";
            cin >> saveEps;
        } while (saveEps != 'y' && saveEps != 'n');
    }

    // Capacidade regulada de acordo com os dados do problema
    capacity = max(int(10 * (numberVertices - 1) / (2 * numberVehicles * fillingCoeff)), 20);

    if (verbosity == 'y')
        cout << "Capacity of each vehicle: " << capacity << endl;

    // Criar um grafo completo respeitando a desigualdade triangular
    graph.createInstance(numberVertices, numRandomClients);

    if (verbosity == 'y')
        graph.printInstance();

    // Executar L-Shaped
    clock_t begin = clock();
    vector<vector<int> > bestSol = solveSVRP(graph, numberVehicles, capacity, 0);
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    double time = 0, cost = 0;

    // Executar busca tabu
    TabuSearchSVRP ts;
    begin = clock();
    svrpSol bestTabuSol = ts.run(graph, numberVehicles, capacity);
    end = clock();
    cost += bestTabuSol.expectedCost;
    time += double(end - begin) / CLOCKS_PER_SEC;

    string nameTabuOutputFile = "output/";
    nameTabuOutputFile += "bestTabuSolN" + to_string(numberVertices)
        + "M" + to_string(numberVehicles)
        + "f" + to_string(fillingCoeff).substr(0, 4);

    string nameBestOutputFile = "output/";
    nameBestOutputFile += "bestSolN" + to_string(numberVertices)
        + "M" + to_string(numberVehicles)
        + "f" + to_string(fillingCoeff).substr(0, 4);

    // Salvar output em um arquivo
    if (saveFile == 'y') {

        string txtNameOutputFile = nameTabuOutputFile + ".txt";
        ofstream outputFile;
        outputFile.open(txtNameOutputFile, std::ios::app);

        if (outputFile.is_open()) {

            if (bestTabuSol.routes.size() == 0)
                outputFile << "Nenhuma solucao viavel encontrada" << endl;

            else {

                for (int i = 0; i < bestTabuSol.routes.size(); i++) {
                    outputFile << "Rota " << i << ": ";
                    for (int j = 0; j < bestTabuSol.routes[i].size(); j++) {
                        outputFile << bestTabuSol.routes[i][j] << " ";
                    }
                    outputFile << endl;
                }
            }

            outputFile << "Custo total: " << bestTabuSol.expectedCost << endl;
            outputFile << "Tempo de processamento: " << elapsed_secs << endl
                       << endl;
            outputFile.close();
        }

        else
            cout << "Unable to open file";

        txtNameOutputFile = nameBestOutputFile + ".txt";
        outputFile;
        outputFile.open(txtNameOutputFile, std::ios::app);

        if (outputFile.is_open()) {

            if (bestSol.size() == 0)
                outputFile << "Nenhuma solucao viavel encontrada" << endl;

            else {
                for (int i = 0; i < bestSol.size(); i++) {
                    outputFile << "Rota " << i << ": ";
                    for (int j = 0; j < bestSol[i].size(); j++) {
                        outputFile << bestSol[i][j] << " ";
                    }
                    outputFile << endl;
                }
            }

            outputFile << "Custo total: " << Bertsimas2ndStage::totalExpectedLength(graph, capacity, bestSol) << endl;
            outputFile << "Tempo de processamento: " << elapsed_secs << endl
                       << endl;
            outputFile.close();
        }

        else
            cout << "Unable to open file";
    }

    // Imprimir solução no stdout
    else {

        if (bestTabuSol.routes.size() == 0)
            cout << "Nenhuma solucao viavel encontrada" << endl;

        else {
            for (int i = 0; i < bestTabuSol.routes.size(); i++) {
                cout << "Rota " << i << ": ";
                for (int j = 0; j < bestTabuSol.routes[i].size(); j++) {
                    cout << bestTabuSol.routes[i][j] << " ";
                }
                cout << endl;
            }
        }

        cout << "Custo total tabu: " << cost << endl;
        cout << "Tempo de processamento: " << time << endl
             << endl;

        if (bestSol.size() == 0)
            cout << "Nenhuma solucao viavel encontrada" << endl;

        else {
            for (int i = 0; i < bestSol.size(); i++) {
                cout << "Rota " << i << ": ";
                for (int j = 0; j < bestSol[i].size(); j++) {
                    cout << bestSol[i][j] << " ";
                }
                cout << endl;
            }
        }

        cout << "Custo total: " << Bertsimas2ndStage::totalExpectedLength(graph, capacity, bestSol) << endl;
        cout << "Tempo de processamento: " << elapsed_secs << endl
             << endl;
    }

    // Salvar imagem EPS com as rotas planejadas
    if (saveEps == 'y') {

        nameTabuOutputFile += ".eps";
        nameBestOutputFile += ".eps";

        if (bestTabuSol.routes.size() != 0)
            drawRoutes(graph, bestTabuSol.routes, nameTabuOutputFile);

        if (bestSol.size() != 0)
            drawRoutes(graph, bestSol, nameBestOutputFile);
    }

    for (int i = 0; i < numberVertices; i++) {
        cout << graph.vertices[i].probOfPresence << " ";
    }
    cout << endl;

    return 0;
}
