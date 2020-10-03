#include "LShapedSVRP.h"

#define INVALID -1

/* Subclasse da classe abstrata GRBCallback. Um objeto da mesma é criado antes da otimização
 * ser ativada, de forma que a implementação do método callback() é chamada periodicamente
 * para adicionar restrições violadas do modelo. */
class DynamicCuts : public GRBCallback {

public:
    GRBVar* x; // variáveis x PLI (arestas)
    GRBVar* objLB; // variável theta PLI (limite inferior ao opt.)
    double L; // limite inferior p/ solução
    Graph g; // grafo de entrada
    int Q; // capacidade do veículo
    int m; // número de veículos

    DynamicCuts(GRBVar* xsol, GRBVar* objLowerBound, double lowerBound,
        Graph graph, int capacity, int numVehicles)
    {
        x = xsol;
        objLB = objLowerBound;
        L = lowerBound;
        g = graph;
        Q = capacity;
        m = numVehicles;
    }

protected:
    // A callback monitora o progresso da otimização, adicionando cortes se preciso
    void callback()
    {
        try {
            if (where == GRB_CB_MIPSOL) {
                int n = g.numberVertices;
                double* xdouble = new double[n * (n - 1) / 2];

                /* Armazenar variáveis x em doubles (gurobi pode retornar 0.999 ao invés
                 * de 1, por exemplo) */
                xdouble = getSolution(x, n * (n - 1) / 2);

                // Armazenar arestas na solução
                vector<edge> edgesInSol;
                for (int e = 0; e < n * (n - 1) / 2; e++) {
                    if (xdouble[e] > 0.5) {
                        edgesInSol.push_back(g.edges[e]);
                    }
                }

                // Criar lista de adjacência do grafo da solução
                vector<list<int> > adjListSol = createAdjList(edgesInSol, n);

                /* Armazenar componentes conexas desse grafo sem o depósito 
                 * (atenção pra ordem em que é retornada) */
                vector<vector<int> > componentsSol = connectedComponents(adjListSol, n);

                // Verificar se componentes violam restrição
                bool violateSubtourConstr = false;

                for (vector<int> cc : componentsSol) {

                    double componentExpectedDemand = 0;
                    int numEdgesInComponent = 0;
                    GRBLinExpr lhs = 0;

                    /* Acumular demanda esperada de todos os clientes e armazenar
                     * arestas da componente */
                    for (int i = 0; i < cc.size(); i++) {

                        int nextClient;
                        list<int> adjs = adjListSol[cc[i]];

                        // Próximo na componente
                        if (i < cc.size() - 1) {
                            nextClient = cc[i + 1];
                            numEdgesInComponent++;
                        }

                        // Verificar se próximo é o primeiro (ciclo)
                        else if (cc.size() > 2 && find(adjs.begin(), adjs.end(), cc[0]) != adjs.end()) {
                            nextClient = cc[0];
                            numEdgesInComponent++;
                        }
                        // Se não for ciclo, marcar próximo com -1
                        else {
                            nextClient = INVALID;
                        }

                        // Percorrer arestas para encontrar a que está sendo analisada (se houver)
                        int currEdge = INVALID;
                        if (nextClient != INVALID) {
                            for (int e = 0; e < n * (n - 1) / 2; e++) {
                                if ((g.edges[e].u == cc[i] && g.edges[e].v == nextClient) || (g.edges[e].u == nextClient && g.edges[e].v == cc[i])) {
                                    currEdge = e;
                                    break;
                                }
                            }
                        }

                        // Colocar aresta na expressão
                        if (currEdge != INVALID) {
                            lhs += x[currEdge];
                        }

                        componentExpectedDemand += g.expectedDemand[cc[i]];
                    }

                    // Adicionar corte no modelo se solução violar restrição
                    if (numEdgesInComponent > cc.size() - 1) {
                        /* Restrição 3 (eliminação de subciclo):
                         * a solução não pode ter subciclo e a demanda esperada de seus vértices
                         * não pode exceder a capacidade do veículo */
                        addLazy(lhs <= cc.size() - 1);
                        violateSubtourConstr = true;
                    }
                }

                /* Verificar se solução é ótima ou adicionar cortes de optimalidade caso
                não for, se não violar nenhuma eliminação de subciclo */
                if (!violateSubtourConstr) {

                    // Armazenar custo esperado do segundo estágio
                    double expectedCost = Bertsimas2ndStage::totalExpectedLength(g, Q, componentsSol);

                    // Armazenar valor da solução encontrada
                    double objValue = getDoubleInfo(GRB_CB_MIPSOL_OBJ);

                    // Verificar se viola objValue >= expectedCost
                    if (objValue < expectedCost) {

                        GRBLinExpr lhs = objLB[0];
                        GRBLinExpr rhs = 0;
                        
                        // Construir e adicionar corte de optimalidade
                        for (int e = 0; e < n * (n - 1) / 2; e++) {
                            if (!(g.edges[e].u == 0 || g.edges[e].v == 0) && xdouble[e] > 0.5) {
                                rhs += x[e];
                            }
                        }
                        rhs += m + 2 - n;
                        rhs *= expectedCost;
                        addLazy(lhs >= rhs);
                    }
                }
            }
        }
        catch (GRBException e) {
            cout << "Error number: " << e.getErrorCode() << endl;
            cout << e.getMessage() << endl;
        }
        catch (...) {
            cout << "Error during callback" << endl;
        }
    }
};

/* Método que resolve o SVRP de forma exata, usando o algoritmo L-Shaped Inteiro
 * inicialmente proposto por Laporte & Louveaux. O objetivo no primeiro estágio é
 * projetar m rotas (uma para cada veículo) de custo esperado mínimo saindo e
 * voltando para o depósito, tal que cada cliente esteja em exatamente uma rota e
 * a demanda esperada de cada rota seja no máximo Q. */
vector<vector<int> > solveSVRP(Graph g, int m, int Q, int L)
{

    GRBEnv* env = NULL;
    GRBVar* x = NULL;
    GRBVar* objLowerBound = NULL;

    // n é o número de clientes + o depósito
    int n = g.numberVertices;

    /* Variável x[e] para cada uma das n*(n-1)/2 arestas e={u,v} do grafo completo
     * não direcionado */
    x = new GRBVar[n * (n - 1) / 2];

    // Limite inferior que substitui o custo esperado na função objetivo
    objLowerBound = new GRBVar[1];

    try {

        // Inicializar ambiente e modelo
        env = new GRBEnv();
        GRBModel svrp = GRBModel(*env);

        // Indicar que restrições serão dinamicamente adicionadas na otimização
        svrp.set(GRB_IntParam_LazyConstraints, 1);

        /* Conhecido um limite inferior do custo esperado, sabe-se que a solução
         * ótima não pode ser menor que este */
        objLowerBound[0] = svrp.addVar(
            L, GRB_INFINITY,
            /* A função objetivo na relaxação do problema consiste em minimizar
             * esse lower da função objetivo original */
            1.0, GRB_CONTINUOUS, "objLowerBound");

        for (int e = 0; e < n * (n - 1) / 2; e++) {

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
                "x[{" + to_string(u) + "," + to_string(v) + "}]");
        }

        /* Restrição 1 (m rotas):
         * as m rotas devem começar e terminar no depósito (vértice 0);
         * ou seja, as arestas devem incidir no depósito 2*m vezes */

        GRBLinExpr lhs = 0;
        for (int e = 0; e < n * (n - 1) / 2; e++) {

            // Salvar vértices da aresta
            int u = g.edges[e].u, v = g.edges[e].v;

            // Arestas incidentes ao depósito
            if (u == 0 || v == 0) {
                lhs += x[e];
            }
        }

        svrp.addConstr(lhs == 2 * m, "m_routes");

        for (int i = 1; i < n; i++) {

            /* Restrição 2 (grau 2):
             * cada cliente i faz parte de uma rota com duas arestas incidindo nele. */

            GRBLinExpr lhs = 0;
            for (int e = 0; e < n * (n - 1) / 2; e++) {

                // Salvar vértices da aresta
                int u = g.edges[e].u, v = g.edges[e].v;

                // Arestas incidentes à i
                if (u == i || v == i) {
                    lhs += x[e];
                }
            }

            svrp.addConstr(lhs == 2, "degr2_" + to_string(i));
        }

        // Definir a callback chamada periodicamente na otimização do modelo
        DynamicCuts cb = DynamicCuts(x, objLowerBound, L, g, Q, m);
        svrp.setCallback(&cb);

        // Escrever modelo inicial no arquivo svrp.lp
        svrp.update();
        svrp.write("svrp.lp");

        // Otimizar
        svrp.optimize();

        if (svrp.get(GRB_IntAttr_SolCount) > 0) {
            double* xdouble = new double[n * (n - 1) / 2];

            /* Armazenar variáveis x em doubles (gurobi pode retornar 0.999 ao invés
            de 1, por exemplo) */
            xdouble = svrp.get(GRB_DoubleAttr_X, x, n * (n - 1) / 2);

            /* Armazenar arestas na solução, tirando as arestas
            incidentes ao depósito */
            vector<edge> edgesInSol;
            for (int e = 0; e < n * (n - 1) / 2; e++) {
                cout << "x[" << g.edges[e].u << "," << g.edges[e].v << "]:" << xdouble[e] << " ";

                if (xdouble[e] > 0.5) {
                    edgesInSol.push_back(g.edges[e]);
                }
            }

            // Criar lista de adjacência do grafo da solução
            vector<list<int> > adjListSol = createAdjList(edgesInSol, n);
            vector<vector<int> > componentsSol = connectedComponents(adjListSol, n);
            return componentsSol;
        }
        // Sem solução
        else
            return 0;
    }
    catch (GRBException e) {
        cout << "Error number: " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    }
    catch (...) {
        cout << "Error during optimization." << endl;
    }
}
