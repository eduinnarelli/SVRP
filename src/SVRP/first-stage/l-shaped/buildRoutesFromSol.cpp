#include "LShapedSVRP.h"

vector<vector<int>> buildRoutesFromSol(edge *edgesInSol, int n) {
    vector<vector<int>> routes;
    vector<bool> seen(n, false);
    
    seen[0] = true;

    // Varrer aresta




}

// /* buildRoutesFromSol: Constrói vetores de rotas, conforme a ordem em
//  * que os vértices são visitados, a partir de uma solução inteira 
//  * do PLI. */
// vector<vector<int>> buildRoutesWithoutDepot(double *sol, Graph g, int n) {

//     vector<vector<int>> routes;
//     vector<bool> seen(n, false);
//     edge edges = g.edges;

//     // O depósito é considerado como visitado
//     seen[0] = true;

//     // Varrer arestas
//     for (int edge = 0; edge < n*(n-1)/2; edge++) {

//         /* Construir rota a partir do próximo vértice adjascente ao depósito
//          * que ainda não foi visitado */
//         if (sol[edge] > 0.5 && !seen[edge.u] ) {

//             vector<int> route;
            
//             // Adicionar primeiro cliente na rota
//             route.push_back(first);
//             seen[first] = true;

//             // Construir rota a partir desse cliente
//             int current = first;
//             for (int next = 1; next < n; next++) {

//                 // Encontrar próximo na rota
//                 if (sol[current][next] > 0.5 && !seen[next]) {

//                     // Adicioná-lo
//                     route.push_back(next);
//                     seen[next] = true;

//                     // Atualizar cliente atual e resetar próximo
//                     current = next;
//                     next = 1;
                    
//                 }
//             }

//             // Rota construída
//             routes.push_back(route);
//         }
//     }

//     return routes;

// }