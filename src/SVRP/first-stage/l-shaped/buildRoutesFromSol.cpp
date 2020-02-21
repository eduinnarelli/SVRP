#include "LShapedSVRP.h"

/* buildRoutesFromSol: Constrói vetores de rotas, conforme a ordem em
 * que os vértices são visitados, a partir de uma solução inteira 
 * do PLI. */
vector<vector<int>> buildRoutesFromSol(double **sol, int n) {

    vector<vector<int>> routes;
    vector<bool> seen(n, false);
    seen[0] = true;

    for (int first = 1; first < n; first++) {

        /* Construir rota a partir do próximo vértice adjascente ao depósito
         * que ainda não foi visitado */
        if (sol[0][first] > 0.5 && !seen[first]) {

            vector<int> route;
            
            // Adicionar primeiro cliente na rota
            route.push_back(first);
            seen[first] = true;

            // Construir rota a partir desse cliente
            int current = first;
            for (int next = 1; next < n; next++) {

                // Encontrar próximo na rota
                if (sol[current][next] > 0.5 && !seen[next]) {

                    // Adicioná-lo
                    route.push_back(next);
                    seen[next] = true;

                    // Atualizar cliente atual e resetar próximo
                    current = next;
                    next = 1;
                    
                }
            }

            // Rota construída
            routes.push_back(route);
        }
    }

    return routes;

}