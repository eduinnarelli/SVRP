#include "LShapedSVRP.h"

/*********************** Cortes de optimalidade *********************** 
 * Quando uma solução viável inteira é encontrada para a relaxação do
 * problema, checamos se seu valor é estritamente menor que o custo 
 * esperado da rota. Em caso positivo, essa solução é inviável para o 
 * problema original e impomos um corte de optimalidade que a elimina. */
class optimalityCut : public GRBCallback
{
    public:
        GRBVar **xvars;
        int n, Q;
        double L;
        Graph g;

        optimalityCut(GRBVar** xvars, double lowerBound, Graph graph, int capacity) {
            xvars = xvars;
            L = lowerBound;
            g = graph;
            Q = capacity;
        }

    protected:
        void callback() {
            try {
                if (where == GRB_CB_MIPSOL) {
                    /* Solução viável inteira encontrada. Armazenar solução
                    em um double (gurobi pode retornar 0.999 ao invés de 1,
                    por exemplo) */
                    double **xsol = new double*[n];
                    for (int i = 0; i < n; i++)
                        xsol[i] = getSolution(xvars[i], n);

                    // Armazenar custo esperado do segundo estágio
                    vector<vector<int>> routes = buildRoutesFromSol(xsol, n);
                    double expectedCost = Bertsimas2ndStage::totalExpectedLength(g, Q, routes);

                    // Armazenar valor da solução encontrada
                    double objValue = getDoubleInfo(GRB_CB_MIPSOL_OBJ);

                    // Verificar se viola objValue >= expectedCost
                    if (objValue < expectedCost) {

                        GRBLinExpr expr = 0;
                        int rhs = 0;

                        /* Corte de optimalidade 
                         * sum(x[i][j], xsol[i][j] = 1) <= sum(xsol[i][j]) - 1 */
                        for (int i = 0; i < n; i++) {
                            for (int j = i+1; j < n; j++) {
                                if (xsol[i][j] > 0.5) {
                                    expr += xvars[i][j];
                                    rhs += 1;
                                }
                            }
                        }

                        addLazy(expr <= rhs - 1);

                    } 
                }
            } catch (GRBException e) {
                cout << "Error number: " << e.getErrorCode() << endl;
                cout << e.getMessage() << endl;
            } catch (...) {
                cout << "Error during callback" << endl;
            }

        }

};

void solveSVRP(Graph g, int m, int Q) {

    GRBEnv *env = NULL;
    GRBVar **x = NULL;
    GRBVar *u = NULL;

    int n = g.numberVertices;

    x = new GRBVar*[n];
    for (int i = 0; i < n; i++)
        x[i] = new GRBVar[n];

    u = new GRBVar[n];

    try {

        // Inicializar ambiente e modelo
        env = new GRBEnv();
        GRBModel model = GRBModel(*env);

        // Indicar que restrições do modelo serão adicionadas via callback
        model.set(GRB_IntParam_LazyConstraints, 1);

        /* Criar variáveis binárias x[i][j] para cada aresta (i,j) e contínuas
         * u[i] para cada vértice i */
        for (int i = 0; i < n; i++) {

            u[i] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "u_"+to_string(i));

            for (int j = 0; j < i; j++) {

                /* Arestas (0,j) incidentes ao depósito podem ser paralelas,
                 * caso em que x[0][j] é 2 na solução. As restantes são 
                 * binárias. */
                double var_bound = (i == 0) ? 2.0 : 1.0;
                
                /* Cada variável recebe um custo associado que corresponde à distância 
                 * entre os pontos i e j. */
                x[i][j] = model.addVar(
                    0.0, var_bound, 
                    g.adjMatrix[i][j],
                    GRB_INTEGER, 
                    "x_"+to_string(i)+"_"+to_string(j)
                );

                // Grafo é não-direcionado
                x[j][i] = x[i][j];

            }
        }

        GRBLinExpr expr = 0;
        for (int j = 1; j < n; j++)
            expr += x[0][j];

        /* Restrição sum(x[0][j]) = 2*m 
         * - as m rotas devem começar e terminar no depósito. */
        model.addConstr(expr == 2*m, "m_routes");

        for (int i = 1; i < n; i++) {

            GRBLinExpr expr = 0;
            for (int j = 0; j < n; j++)
                expr += x[i][j];

            /* Restrição sum(x[i][j]) = 2 para todo i > 0 fixo:
            * - cada cliente faz parte de uma rota com duas arestas incidindo sobre ele. */
            model.addConstr(expr == 2, "degr2_" + to_string(i));

            for (int j = 1; j < n; j++) {

                /* Restrição u[i] + q[j] = u[j] se x[i][j] = 1 para todo i != 0, j != 0:
                * - eliminação de subciclos (MTZ); q[j] é a demanda média do cliente j. */
                model.addGenConstrIndicator(
                    x[i][j], true, // indicator x[i][j] == 1
                    u[i] + g.expectedDemand[j] == u[j],
                    "subtourelim_" + to_string(i) + "_" + to_string(j)
                );

            }

            /* Restrição u[i] >= q[i] para todo cliente:
             * - a demanda de i deve ser atendida. */
            model.addConstr(u[i] >= g.expectedDemand[i], "met_demand_" + to_string(i));

            /* Restrição u[i] <= Q para todo cliente:
             * - a capacidade do veículo não pode ser ultrapassada. */
            model.addConstr(u[i] <= Q, "capacity_" + to_string(i));

        }

        // Definir callback
        optimalityCut cb = optimalityCut(x, 0, g, Q);
        model.setCallback(&cb);

        // Otimizar modelo
        model.optimize();

    } catch (GRBException e) {
        cout << "Error number: " << e.getErrorCode << endl;
        cout << e.getMessage << endl;
    } catch (...) {
        cout << "Error during optimization." << endl;
    }

}