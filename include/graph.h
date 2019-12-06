#include <lemon/math.h>
#include<lemon/graph_to_eps.h>
#include<lemon/list_graph.h>
#include<iostream>
#include<algorithm>
#include<cmath>
#include<vector>
#include<random>
#include<string>
#include<time.h>
using namespace std;
using namespace lemon;
extern char verbosity;

struct vertex {

    double x, y;
    double probDemand[21];
    double probOfPresence = 1;
};

struct edge {

    int u, v;
    double dist;
};

class Graph {

public:

    int numberVertices;
    double totalExpectedDemand;
    vector<double> expectedDemand;
    vector<vertex> vertices;
    vector<vector<double>> adjMatrix;

    void createInstance(int n);
    void computeDistances();
    void printInstance();
    void drawGraph(string graphName);
    vector<int> TSP();

};
