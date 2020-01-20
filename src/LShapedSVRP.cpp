#include "LShapedSVRP.h"

// https://www.gurobi.com/documentation/9.0/examples/tsp_cpp_cpp.html#subsubsection:tsp_c++.cpp

void solveSVRP(Graph g, int m) {

    GRBEnv *env = NULL;
    GRBVar **vars = NULL;

    int n = g.numberVertices;

    vars = new GRBVar*[n];
    for (int i = 0; i < n; i++)
        vars[i] = new GRBVar[n];

    try {

        // Inicializar ambiente e modelo
        env = new GRBEnv();
        GRBModel model = GRBModel(*env);

        // Criar variáveis binárias x[i][j] para cada aresta (i,j)
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= i; j++) {

                /* Arestas (0,j) incidentes ao depósito podem ser paralelas,
                 * caso em que x[0][j] é 2 na solução. As restantes são 
                 * binárias. */
                double var_bound = (i == 0) ? 2.0 : 1.0;
                
                if (i != j) {

                    /* Cada variável recebe um custo associado
                    * que corresponde à distância entre os pontos
                    * i e j. */
                    vars[i][j] = model.addVar(
                        0.0, var_bound, 
                        g.adjMatrix[i][j],
                        GRB_INTEGER, 
                        "x_"+to_string(i)+"_"+to_string(j)
                    );

                }

                // Grafo é não-direcionado
                vars[j][i] = vars[i][j];

            }
        }

        /* Restrição sum(x[0][j]) = 2*m 
         * - as m rotas devem começar e terminar no depósito. */

        GRBLinExpr expr = 0;
        for (int j = 1; j < n; j++)
            expr += vars[0][j];

        model.addConstr(expr == 2*m, "m_routes");

        /* Restrição sum(x[i][j]) = 2 para todo i > 0 fixo:
         * - cada cliente faz parte de uma rota com duas arestas
         *   incidindo sobre ele. */

        for (int i = 1; i < n; i++) {

            GRBLinExpr expr = 0;
            for (int j = 0; j < n; j++)
                expr += vars[i][j];

            model.addConstr(expr == 2, "degr2_" + to_string(i));

        }





        





    } catch (GRBException e) {
        cout << "Error number: " << e.getErrorCode << endl;
        cout << e.getMessage << endl;
    } catch (...) {
        cout << "Error during optimization." << endl;
    }


}