#include<iostream>
#include<vector>
using namespace std;

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
    vector<int> TSP();

};
