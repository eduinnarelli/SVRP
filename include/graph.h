#include<iostream>
#include<vector>
using namespace std;

struct vertex {
    int demRange;
    double probOfPresence = 1;
    double x, y;
    double probDemand[21];
};

struct edge {
    int u, v;
    double dist;
};

class Graph {

public:

    int numberVertices;
    vector<vertex> vertices;
    vector<vector<double>> adjMatrix;
    
    void createInstance(int n);
    void computeDistances();
    void printInstance();
    vector<int> TSP();

};
