#include "SVRP.h"

// Retornar componentes conexas de um grafo não-direcionado
vector<vector<int> > connectedComponents(vector<list<int> > adjList, int n)
{

    vector<vector<int> > components;

    // Marcar vértices como não visitados
    vector<bool> visited(n, false);

    // Primeiro, mapear componentes adjacentes ao depósito
    for (int v = 1; v < n; v++) {

        /* Se v não foi visitado e é adjacente ao depósito, é necessário construir 
         * sua componente (respeitando a ordem do percurso) */
        if (!visited[v] && find(adjList[0].begin(), adjList[0].end(), v) != adjList[0].end()) {

            vector<int> component;

            // Componente é armazenada em 'component'
            DFSComponent(v, adjList, visited, component);
            components.push_back(component);
        }
    }

    // Repetir processo pra vértices não visitados e não adjacentes ao depósito (ciclos)
    for (int v = 1; v < n; v++) {
        if (!visited[v]) {
            vector<int> component;
            DFSComponent(v, adjList, visited, component);
            components.push_back(component);
        }
    }

    return components;
}

// Construir componente por busca em profundidade
void DFSComponent(int v, vector<list<int> > adjList, vector<bool>& visited, vector<int>& component)
{

    // v foi visitado e pertence à componente atual
    visited[v] = true;
    component.push_back(v);

    // Iterar sob os adjacentes de v
    list<int>::iterator it;
    for (it = adjList[v].begin(); it != adjList[v].end(); ++it) {
        if (!visited[*it] && *it != 0) {
            // Continuar do próximo não visitado
            DFSComponent(*it, adjList, visited, component);
        }
    }
}
