#include "SVRP.h"

/* Etapas 3 e 4: Atualizar estruturas, parâmetros e melhor solução conforme
a viabilidade da solução corrente. */
void TabuSearchSVRP::update()
{

    if (verbosity == 'y')
        cout << "UPDATE" << endl;

    if (this->moveDone.valid) {

        this->numRoutes = 0;
        for (int i = 0; i < this->sol.routes.size(); i++) {
            if (!this->sol.routes[i].empty())
                this->numRoutes++;
        }

        if (verbosity == 'y')
            cout << "Movimento escolhido: " << this->moveDone.client << " " << this->moveDone.neighbour << endl;

        if (verbosity == 'y')
            cout << "Numero de rotas atual: " << this->numRoutes << endl;

        // Checar se ao menos melhorou
        if (this->sol.expectedCost < this->bestPenalExpCost) {

            if (verbosity == 'y')
                cout << "Solucao melhorou (F(x) < F*)" << endl;

            this->bestPenalExpCost = this->sol.expectedCost;
            this->currNoImprovement = 0;
        }

        // Inviável
        if (this->numRoutes != this->numVehicles) {

            if (verbosity == 'y')
                cout << "Solucao inviavel" << endl;

            this->numInfeasibleNearby += 1;

            // Viável
        }
        else {
            this->numInfeasibleNearby = 0;

            if (verbosity == 'y')
                cout << "Solucao viavel" << endl;

            if (this->sol.expectedCost < this->bestFeasibleSol.expectedCost) {

                if (verbosity == 'y')
                    cout << "Solucao viavel melhorou (F(x)=T(x) < T*)" << endl;

                this->bestFeasibleSol.expectedCost = this->sol.expectedCost;
                this->bestFeasibleSol = this->sol;
            }
        }

        routeOfClient[moveDone.client] = routeOfClient[moveDone.neighbour];
        if (this->g.numberVertices > 5) {
            this->moveDone.tabuDuration = this->itCount + (this->g.numberVertices - 5) + (rand() % 6);
        }
        else {
            this->moveDone.tabuDuration = this->itCount + (1) + (rand() % 6);
        }

        // Inserir movimento realizado
        this->tabuMoves.push_back(this->moveDone);
    }

    else if (verbosity == 'y')
        cout << "Todos os movimentos sao tabu e nenhum melhora" << endl;

    for (int i = 0; i < this->tabuMoves.size(); i++) {

        // Remover da lista de movimentos tabus se ultrapassou duração
        if (this->itCount >= tabuMoves[i].tabuDuration) {

            routeMove toBeErased = this->tabuMoves[i];

            this->tabuMoves.erase(remove_if(this->tabuMoves.begin(), this->tabuMoves.end(), [toBeErased](routeMove m) {
                                      return m == toBeErased;
                                  }), this->tabuMoves.end());
        }
    }

    // Atualizar penalidade
    if (this->itCount % 10 == 0) {
        this->penalty *= pow(2, this->numInfeasibleNearby / 5 - 1);
        this->numInfeasibleNearby = 0;
    }
}
