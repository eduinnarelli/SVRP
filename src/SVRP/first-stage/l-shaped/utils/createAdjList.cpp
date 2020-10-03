#include "SVRP.h"

// Criar lista de adjacência a partir de conjunto de arestas
vector<list<int> > createAdjList(vector<edge> edges, int n)
{

    vector<list<int> > adj(n);

    // Varrer arestas
    for (edge e : edges) {
        int u = e.u, v = e.v;

        // Armazenar adjacências
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    return adj;
}
