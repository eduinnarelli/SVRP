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
