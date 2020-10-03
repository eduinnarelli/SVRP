#include "graph.h"

#define MAX_ITERATIONS 10000

/*
Lista de definições e especificações:

- routeMove: Estrutura que indica o movimento sendo realizado ao procurar
soluções vizinhas. O movimento é levar "client" para a posição anterior à
"neighbour". "it" possui o número da iteração na qual esse movimento foi
realizado, utilizado para manter os movimentos tabu. Um routeMove é considerado
igual à outro se "client" e "neighbour" forem iguais.

- svrpSol: Estrutura que indica uma solução do svrp com rotas "routes" e custo
total "expectedCost".

- g: Grafo do problema.

- penalty: penalidade para soluções inviáveis (numRoutes > numVehicles).

- bestPenalExpCost: melhor custo de solução encontrado até o momento, seja esta
solução viável ou não. É igual a variável "F*(x)" do paper.

- numVehicles: número de veículos disponíveis no problema.

- capacity: capacidade dos veículos.

- numSelected: número de movimentos que serão explorados na vizinhança de uma
solução. É igual a variável "q" do paper.

- numNearest: número de clientes considerados próximos à um cliente. É igual a
variável "p" do paper.

- numRoutes: número de rotas na solução atual (x = sol). É
igual a "m^v" do paper.

- itCount: número da iteração atual. É igual a variável "t0" do paper.

- numInfeasibleNearby: número de soluções inviáveis nas 10 últimas iterações.
É igual a variável "t2" do paper.

- currNoImprovement: número de iterações sem melhora na solução ótima
(x* = bestFeasibleSol). É igual a variável "t1" do paper.

- maxNoImprovement: número de iterações máxima permitida sem melhora na solução
ótima. É igual a variável "t3" do paper.

- moveDone: melhor movimento não-tabu selecionado na lista de candidatos
bestMoves (= variável L do paper) que gera a solução sol (="y" ou "z" do paper).

- routeOfClient: vetor de tamanho this->g.numberVertices, ou seja, igual ao
número de vértices. Cada posição i do vetor contém o número da rota do cliente
i.

- relativeDemand: vetor de tamanho this->g.numberVertices-1, ou seja, igual ao
número de clientes. Cada posição i do vetor contém a demanda relativa do cliente
i - 1.

- closestNeighbours: vetor de vetores de inteiros de tamanho
this->g.numberVertices x h = min(this->g.numberVertices - 1, 10). Cada linha i possui um vetor
que armazena os vértices mais próximos do vértice i. Cada coluna j é o j-ésimo
vizinho mais próximo do vértice i. O depósito não é considerado vizinho de
nenhum vértice.

- tabuMoves: vetor de routeMoves que armazena os movimentos considerados tabu.

- sol: melhor solução encontrada na iteração atual. É igual a variável
"x" do paper.

- bestFeasibleSol: melhor solução viável encontrada. É igual a variável
"T*" do paper.
*/
struct routeMove {

    int client, neighbour, tabuDuration, clientRoute;
    double approxCost;
    bool valid;

    bool operator==(const routeMove& other) const
    {
        return this->client == other.client
            && this->clientRoute == other.clientRoute;
    }
};

struct svrpSol {
    vector<vector<int> > routes;
    double expectedCost;
};

class TabuSearchSVRP {

    Graph g;
    long double penalty, bestPenalExpCost;
    int numVehicles, capacity, numSelected, numNearest, numRoutes;
    int itCount, numInfeasibleNearby;
    int currNoImprovement, maxNoImprovement;
    routeMove moveDone;
    vector<int> routeOfClient;
    vector<double> relativeDemand;
    vector<vector<int> > closestNeighbours;
    vector<routeMove> tabuMoves;
    svrpSol sol, bestFeasibleSol;

public:
    svrpSol run(Graph inst, int numVehicles, int capacity);

private:
    // Etapas
    void initialize(Graph inst, int numVehicles, int capacity);
    void neighbourhoodSearch();
    void update();
    void intensification();

    // Funções
    double penalizedExpectedLength(vector<vector<int> > sol);
    double removalCost(int r, int client);
    double maxRemovalCost(int r);
    double approxInsertImpact(int a, int b, int c);
    double approxMoveCost(routeMove m);
};
