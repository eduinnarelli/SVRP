#include "graph.h"

/*
createInstance: Cria um grafo completo não-direcionado com "n" vértices
utilizando uma matriz de adjacências e um vetor de vértices.
Cada posição (i,j) da matriz indica a distância de um vértice i ao j.
As distâncias respeitam a desigualdade triangular. No vetor de vértices,
cada vértice possui uma demanda entre 3 possíveis intervalos, [1,9], [5,15]
e [10,20].

Entrada:
n: número de vértices do grafo.
*/
void Graph::createInstance(int n) {

    int range;
    vertex newVertex;
    //std::mt19937 generator(time(0)); // Para instâncias aleatórias
    std::mt19937 generator(7); // Para instâncias fixas

    // Possíveis coordenadas dos vértices e probabilidade de presença
    uniform_real_distribution<double> coordinate(0, 100), presence(0,1);

    // Possíveis intervalos das demandas dos vértices
    uniform_int_distribution<int> demRange(1,3);

    // Possíveis demandas dos vértices para cada intervalo
    uniform_int_distribution<int> demand1(1,9), demand2(5,15), demand3(10,20);

    this->numberVertices = n;
    this->totalExpectedDemand = 0;
    this->expectedDemand.resize(n);
    fill(this->expectedDemand.begin(),this->expectedDemand.end(),0);

    // Inicializar matriz de adjacências
    vector<double> v(n, 0);
    vector<vector<double>> matrix(n, v);
    this->adjMatrix = matrix;

    for (int i = 0; i < this->numberVertices; i++) {

        // Gerar coordenadas dos vértices em [0,100]
        newVertex.x = coordinate(generator);
        newVertex.y = coordinate(generator);
        newVertex.probDemand[0] = 0;

        if (i > 0) {

            // Gerar probabilidade de presença e demanda dos clientes

            newVertex.probOfPresence = presence(generator);
            range = demRange(generator);

            switch(range) {

                // Intervalo [1,9]
                case 1:

                    for (int j = 1; j < 21; j++) {

                        if (j < 10) {
                            newVertex.probDemand[j] = 1.0/9.0;
                        } else {
                            newVertex.probDemand[j] = 0;
                        }

                        this->expectedDemand[i] += newVertex.probDemand[j]*j;

                    }

                    break;

                // Intervalo [5,15]
                case 2:

                    for (int j = 1; j < 21; j++) {

                        if (j >= 5 && j <= 15) {
                            newVertex.probDemand[j] = 1.0/11.0;
                        } else {
                            newVertex.probDemand[j] = 0;
                        }

                        this->expectedDemand[i] += newVertex.probDemand[j]*j;

                    }

                    break;

                // Intervalo [10,20]
                case 3:

                    for (int j = 1; j < 21; j++) {

                        if (j > 9) {
                            newVertex.probDemand[j] = 1.0/11.0;
                        } else {
                            newVertex.probDemand[j] = 0;
                        }

                        this->expectedDemand[i] += newVertex.probDemand[j]*j;

                    }

                default:
                    break;

            }

            this->expectedDemand[i] *= newVertex.probOfPresence;
            this->totalExpectedDemand += this->expectedDemand[i];

        }

        this->vertices.push_back(newVertex);
    }

    computeDistances();

}

/*
computeDistances: Computa as distâncias euclidianas de todos para todos os vértices,
utilizando suas coordenadas.
*/
void Graph::computeDistances() {

    double x1, x2, y1, y2, dist;

    for (int i = 0; i < this->numberVertices; i++) {
        for (int j = 0; j < i; j++) {

            x1 = this->vertices[i].x;
            x2 = this->vertices[j].x;

            y1 = this->vertices[i].y;
            y2 = this->vertices[j].y;

            // Salvar a distância euclidiana entre os vértices
            dist = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
            this->adjMatrix[i][j] = dist;
            this->adjMatrix[j][i] = dist;

        }
    }

}

/*
printInstance: Imprime os seguintes dados do problema:
- Coordenadas do depósito;
- Coordenadas de cada cliente, com suas probabilidades de presença e de demanda;
- Distância de todos os vértices para todos (apenas uma vez, pois a distância de i a j é igual a de j a i;
*/
void Graph::printInstance() {

    cout << "\nDepot in: " << this->vertices[0].x << ' ' << this->vertices[0].y << "\n\n";

    for (int i = 1; i < this->numberVertices; i++) {

        cout << "Client " << i << ": ";
        cout << this->vertices[i].x << ' ' << this->vertices[i].y << endl;
        cout << "Prob. of presence: " << this->vertices[i].probOfPresence << endl;
        cout << "Demand probabilities: ";

        for (int j = 1; j < 21; j++) {
            cout << vertices[i].probDemand[j] << ' ';
        }

        cout << "\n\n";

    }

    for (int i = 0; i < this->numberVertices; i++) {
        for (int j = 0; j < i; j++) {
            cout << "Distance (" << i << "," << j << "): ";
            cout << this->adjMatrix[i][j] << endl;
        }
    }

}

/*
TSP: Algoritmo força bruta que encontra o menor caminho percorrendo todos os vértices,
imprimindo a ordem que os vértices devem ser percorridos e o seu custo.

Saída:
minPath: vetor onde cada posição indica o próximo vértice a ser percorrido no caminho.
*/
vector<int> Graph::TSP() {

    vector<int> vtxRouteOrder, minPath;
    int vtx;
    double pathCost = 0, minPathCost = INT32_MAX;

    for (int i = 1; i < this->numberVertices; i++) {
        vtxRouteOrder.push_back(i);
    }

    do {

        pathCost = 0;
        vtx = 0;

        for (int i = 0; i < this->numberVertices - 1; i++) {
            pathCost += this->adjMatrix[vtx][vtxRouteOrder[i]];
            vtx = vtxRouteOrder[i];
        }

        pathCost += this->adjMatrix[vtx][0];

        if (pathCost < minPathCost) {
            minPathCost = pathCost;
            minPath = vtxRouteOrder;
        }

    } while(next_permutation(vtxRouteOrder.begin(), vtxRouteOrder.end()));

    cout << "Smallest TSP route: ";
    for (int i = 0; i < minPath.size(); i++)
        cout << minPath[i] << ' ';
    cout << endl;

    cout << "Cost of TSP route: " << minPathCost << endl;

    return minPath;

}

void Graph::drawGraph(string graphName) {

  Palette palette;
  Palette paletteW(true);

  ListGraph g;
  typedef ListGraph::Node Node;
  typedef ListGraph::NodeIt NodeIt;
  typedef ListGraph::Edge Edge;
  typedef dim2::Point<int> Point;

  vector<Node> nodes;

  ListGraph::NodeMap<Point> coords(g);
  ListGraph::NodeMap<int> colors(g);
  ListGraph::NodeMap<int> shapes(g);
  ListGraph::EdgeMap<int> ecolors(g);

  for(int i = 0; i < this->numberVertices; i++) {

    nodes.push_back(g.addNode());
    coords[nodes[i]] = Point(this->vertices[i].x, this->vertices[i].y);

  }

  // Destacar deposito
  colors[nodes[0]] = 1;
  shapes[nodes[0]] = 1;

  for(int i = 0; i < this->numberVertices; i++) {

    for(int j = i+1; j < this->numberVertices; j++) {

      if(this->adjMatrix[i][j] >= 0) {

        Edge e = g.addEdge(nodes[i],nodes[j]);
        ecolors[e] = this->adjMatrix[i][j];
      }

    }
  }

  cout << "Create " << graphName << endl;

  graphToEps(g, graphName).
    coords(coords).
    title("Figura do grafo").
    copyright("(C) 2003-2007 LEMON Project").
    nodeColors(composeMap(palette,colors)).
    edgeColors(composeMap(palette,ecolors)).
    run();

}
