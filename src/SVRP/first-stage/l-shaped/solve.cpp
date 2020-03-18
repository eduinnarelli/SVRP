#include "LShapedSVRP.h"

/* Método que resolve o SVRP de forma exata, usando o algoritmo L-Shaped Inteiro
 * inicialmente proposto por Laporte & Louveaux. O objetivo no primeiro estágio é
 * projetar m rotas (uma para cada veículo) de custo esperado mínimo saindo e 
 * voltando para o depósito, tal que cada cliente esteja em exatamente uma rota e 
 * a demanda esperada de cada rota seja no máximo Q. */
void solveSVRP(Graph g, int m, int Q, int L) {

    GRBEnv *env = NULL;
    GRBVar *x = NULL;
    GRBVar *objLowerBound = NULL;

    // n é o número de clientes + o depósito
    int n = g.numberVertices;

    /* Variável x[e] para cada uma das n*(n-1)/2 arestas e={u,v} do grafo completo 
     * não direcionado */
    x = new GRBVar[n*(n-1)/2];

    // Limite inferior que substitui o custo esperado na função objetivo
    objLowerBound = new GRBVar[1];

    try {

        // Inicializar ambiente e modelo
        env = new GRBEnv();
        GRBModel svrp = GRBModel(*env);

        /* Conhecido um limite inferior do custo esperado, sabe-se que a solução
         * ótima não pode ser menor que este */
        objLowerBound[0] = svrp.addVar(
            L, GRB_INFINITY, 
            /* A função objetivo na relaxação do problema consiste em minimizar 
             * esse lower da função objetivo original */
            1.0, GRB_CONTINUOUS, "objLowerBound"
        );

        for (int e = 0; e < n*(n-1)/2 ; e++) {

            // Salvar vértices da aresta
            int u = g.edges[e].u, v = g.edges[e].v;

            /* x é uma variável inteira que representa o número de vezes que a aresta
             * (u,v) aparece na solução. Geralmente x assume 0 ou 1. Arestas incidentes 
             * ao depósito podem ser paralelas, quando uma rota consiste em visitar um 
             * vértice e imediatamente voltar. Nesse caso, x é 2. */
            double upper_bound = (u == 0 || v == 0) ? 2.0 : 1.0;

            x[e] = svrp.addVar(
                // Intervalo de inteiros que a variável pode assumir
                0.0, upper_bound,
                // As variáveis x não constam na função objetivo
                0.0,
                // As variáveis são inteiras
                GRB_INTEGER,
                // Nome x[e={u,v}] da variável
                "x[" + to_string(e) + "={" + to_string(u) + "," + to_string(v) + "}]"
            );
        }

        /* Restrição 1: 
         * as m rotas devem começar e terminar no depósito (vértice 0);
         * ou seja, as arestas devem incidir no depósito 2*m vezes */

        GRBLinExpr lhs = 0;
        for (int e = 0; e < n*(n-1)/2; e++) {

            // Salvar vértices da aresta
            int u = g.edges[e].u, v = g.edges[e].v;

            // Arestas incidentes ao depósito
            if (u == 0 || v == 0) {
                lhs += x[e];
            }
        }

        svrp.addConstr(lhs == 2*m, "m_routes");

        for (int i = 1; i < n; i++) {

            /* Restrição 2:
             * cada cliente i faz parte de uma rota com duas arestas incidindo nele. */

            GRBLinExpr lhs = 0;
            for (int e = 0; e < n*(n-1)/2; e++) {

                // Salvar vértices da aresta
                int u = g.edges[e].u, v = g.edges[e].v;

                // Arestas incidentes à i
                if (u == i || v == i) {
                    lhs += x[e];
                }

            }

            svrp.addConstr(lhs == 2, "degr2_" + to_string(i));

        }
        
        // Primeiro gera subtour nas callbacks, dps optimal cut!!






    } catch (GRBException e) {
        cout << "Error number: " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Error during optimization." << endl;
    }

}