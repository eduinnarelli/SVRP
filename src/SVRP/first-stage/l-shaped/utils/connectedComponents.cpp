#include "SVRP.h"

// Retornar componentes conexas de um grafo não-direcionado
vector<vector<int>> connectedComponents(vector<list<int>> adjList, int n) {

    vector<vector<int>> components;

    // Marcar vértices como não visitados
    vector<bool> visited(n, false);

    // Ignorar vértice 0 (depósito)
    for (int v = 1; v < n; v++) {

        // Se v não foi visitado, é necessário construir sua componente
        if (!visited[v]) {

            vector<int> component;

            // Componente é armazenada em 'component'
            DFSComponent(v, adjList, visited, component);
            components.push_back(component);

        }
    }

    return components;
}

// Construir componente por busca em profundidade
void DFSComponent(int v, vector<list<int>> adjList, vector<bool> &visited, vector<int> &component) {

    // v foi visitado e pertence à componente atual
    visited[v] = true;
    component.push_back(v);

    // Iterar sob os adjacentes de v
    list<int>::iterator it;
    for(it = adjList[v].begin(); it != adjList[v].end(); ++it) {
        if(!visited[*it]) {
            // Continuar do próximo não visitado
            DFSComponent(*it, adjList, visited, component);
        }
    }

}