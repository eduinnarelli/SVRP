#include "SVRP.h"

/* Etapa 2: avaliar soluções a partir de movimentos entre vizinhos */
void TabuSearchSVRP::neighbourhoodSearch()
{

    if (verbosity == 'y')
        cout << "NEIGHBOURHOOD_SEARCH" << endl;

    vector<routeMove> bestMoves;

    this->itCount++;
    this->currNoImprovement++;

    vector<int> customers;
    for (int i = 0; i < this->g.numberVertices - 1; i++) {
        customers.push_back(i + 1);
    }

    // Considerar todos movimentos candidatos na vizinhança
    for (int i = 0; i < this->numSelected; i++) {

        routeMove newMove;

        newMove.client = customers[rand() % (this->g.numberVertices - 1)];
        newMove.valid = true;
        newMove.clientRoute = routeOfClient[newMove.client];

        //Escolher um vizinho desse cliente aleatoriamente
        int neighbourIdx = rand() % this->numNearest;
        newMove.neighbour = this->closestNeighbours[newMove.client][neighbourIdx];

        /* Computar o custo aproximado de se remover o cliente de sua rota e
    	inserí-lo imediatamente ou antes do vizinho escolhido. */

        newMove.approxCost = approxMoveCost(newMove);

        if (verbosity == 'y') {
            cout << "Movimento " << i << ": " << newMove.client << " " << newMove.neighbour << endl;
            cout << "Custo: " << newMove.approxCost << endl;
        }

        // Inserir na lista de movimentos em ordem não-decrescente
        auto pos = find_if(bestMoves.begin(), bestMoves.end(), [newMove](routeMove m) {
            return m.approxCost > newMove.approxCost;
        });

        bestMoves.insert(pos, newMove);
    }

    if (verbosity == 'y')
        cout << "bestMoves: " << endl;

    for (int i = 0; i < bestMoves.size(); i++) {

        if (verbosity == 'y')
            cout << bestMoves[i].approxCost << " ";
    }

    if (verbosity == 'y')
        cout << endl;

    /* Analisar os 5 primeiros movimentos. Se algum deles for melhor que a solução
	atual, mesmo que seja um movimento tabu (por causa do critério de aspiração),
	então o menor destes movimentos será realizado. Se não, o melhor movimento
	não tabu é realizado, analisando em blocos de 5 */
    int numTabuMoves = 0;
    routeMove bestMoveNotTabu;
    double bestMovePenalExpCost = numeric_limits<double>::max(), bestMoveNotTabuPenalExpCost = numeric_limits<double>::max();
    vector<vector<int> > bestRoutes, bestNotTabuRoutes;
    this->moveDone.valid = false;
    bestMoveNotTabu.valid = false;

    for (int i = 0; i < min(5, (int)bestMoves.size()); i++) {

        routeMove currMove = bestMoves[i];

        if (verbosity == 'y')
            cout << "Analisando movimento " << i << ": " << currMove.client << " " << currMove.neighbour << endl;

        // Checar se é um movimento tabu
        auto tabuPos = find_if(tabuMoves.begin(), tabuMoves.end(), [currMove](routeMove m) {
            return m == currMove;
        });

        bool notTabu = (tabuPos == tabuMoves.end());

        vector<vector<int> > actualSol = this->sol.routes;
        double movePenalExpCost;

        if (routeOfClient[currMove.client] == routeOfClient[currMove.neighbour]) {

            vector<int> sameRoute = actualSol[routeOfClient[currMove.client]];

            if (verbosity == 'y')
                cout << "Rota de ambos antes da troca: ";

            for (int j = 0; j < sameRoute.size(); j++) {

                if (verbosity == 'y')
                    cout << sameRoute[j] << " ";
            }

            if (verbosity == 'y')
                cout << endl;

            vector<int>::iterator clientPos, neighbourPos;
            for (int j = 0; j < sameRoute.size(); j++) {
                if (sameRoute[j] == currMove.client) {
                    clientPos = sameRoute.begin() + j;
                }
                else if (sameRoute[j] == currMove.neighbour) {
                    neighbourPos = sameRoute.begin() + j;
                }
            }

            if (clientPos < neighbourPos) {
                sameRoute.erase(remove(sameRoute.begin(), sameRoute.end(), currMove.client));
                sameRoute.insert(neighbourPos - 1, currMove.client);
            }
            else {
                sameRoute.erase(remove(sameRoute.begin(), sameRoute.end(), currMove.client));
                sameRoute.insert(neighbourPos, currMove.client);
            }

            if (verbosity == 'y')
                cout << "Rota de ambos apos a troca: ";

            for (int j = 0; j < sameRoute.size(); j++) {

                if (verbosity == 'y')
                    cout << sameRoute[j] << " ";
            }

            if (verbosity == 'y')
                cout << endl;

            actualSol[routeOfClient[currMove.client]] = sameRoute;

            // Computar custo esperado e armazenar a melhor solução encontrada
            movePenalExpCost = penalizedExpectedLength(actualSol);
        }

        else {

            vector<int> clientRoute = actualSol[routeOfClient[currMove.client]];
            vector<int> neighbourRoute = actualSol[routeOfClient[currMove.neighbour]];

            int neighbour = currMove.neighbour;
            auto neighbourPos = find_if(neighbourRoute.begin(), neighbourRoute.end(), [neighbour](int x) {
                return x == neighbour;
            });

            if (verbosity == 'y')
                cout << "Rota do vizinho antes de ser adicionado: ";

            for (int j = 0; j < neighbourRoute.size(); j++) {

                if (verbosity == 'y')
                    cout << neighbourRoute[j] << " ";
            }

            if (verbosity == 'y')
                cout << endl;

            neighbourRoute.insert(neighbourPos, currMove.client);

            if (verbosity == 'y')
                cout << "Rota do vizinho apos ser adicionado: ";

            for (int j = 0; j < neighbourRoute.size(); j++) {

                if (verbosity == 'y')
                    cout << neighbourRoute[j] << " ";
            }

            if (verbosity == 'y') {
                cout << endl;
            }

            if (verbosity == 'y')
                cout << "Rota do cliente antes de ser removido: ";

            for (int j = 0; j < clientRoute.size(); j++) {

                if (verbosity == 'y')
                    cout << clientRoute[j] << " ";
            }

            if (verbosity == 'y')
                cout << endl;

            // Remover cliente da sua rota e inserir na rota vizinha
            clientRoute.erase(remove(clientRoute.begin(), clientRoute.end(), currMove.client));

            if (verbosity == 'y')
                cout << "Rota do cliente apos ser removido: ";

            for (int j = 0; j < clientRoute.size(); j++) {

                if (verbosity == 'y')
                    cout << clientRoute[j] << " ";
            }

            if (verbosity == 'y')
                cout << endl;

            if (clientRoute.empty())
                this->numRoutes--;

            actualSol[routeOfClient[currMove.client]] = clientRoute;
            actualSol[routeOfClient[currMove.neighbour]] = neighbourRoute;

            // Computar custo esperado e armazenar a melhor solução encontrada
            movePenalExpCost = penalizedExpectedLength(actualSol);

            if (clientRoute.empty())
                this->numRoutes++;
        }

        if (movePenalExpCost < bestMovePenalExpCost) {
            bestMovePenalExpCost = movePenalExpCost;
            bestRoutes = actualSol;
            this->moveDone = currMove;
        }

        if (notTabu) {
            if (movePenalExpCost < bestMoveNotTabuPenalExpCost) {
                bestMoveNotTabuPenalExpCost = movePenalExpCost;
                bestNotTabuRoutes = actualSol;
                bestMoveNotTabu = currMove;
            }
        }
    }

    /* Possível critério de aspiração */
    if (bestMovePenalExpCost < sol.expectedCost) {
        sol.expectedCost = bestMovePenalExpCost;
        sol.routes = bestRoutes;
        return;
    }

    else if (numTabuMoves == 5) {

        numTabuMoves = 0;
        bestMovePenalExpCost = numeric_limits<double>::max();
        this->moveDone.valid = false;
        bestMoveNotTabu.valid = false;

        for (int i = 5; i < bestMoves.size(); i++) {

            if (i > 5 && i % 5 == 0 && numTabuMoves != 5)
                break;
            else if (i % 5 == 0 && numTabuMoves == 5)
                numTabuMoves = 0;

            routeMove currMove = bestMoves[i];

            if (verbosity == 'y')
                cout << "Analisando movimento " << i << ": " << currMove.client << " " << currMove.neighbour << endl;

            // Checar se é um movimento tabu
            auto tabuPos = find_if(tabuMoves.begin(), tabuMoves.end(), [currMove](routeMove m) {
                return m == currMove;
            });

            if (tabuPos != tabuMoves.end()) {
                numTabuMoves++;
                continue;
            }

            vector<vector<int> > actualSol = this->sol.routes;

            vector<int> clientRoute = actualSol[routeOfClient[currMove.client]];
            vector<int> neighbourRoute = actualSol[routeOfClient[currMove.neighbour]];

            int neighbour = currMove.neighbour;
            auto neighbourPos = find_if(neighbourRoute.begin(), neighbourRoute.end(), [neighbour](int x) {
                return x == neighbour;
            });

            if (verbosity == 'y')
                cout << "Rota do vizinho antes de ser adicionado: ";

            for (int j = 0; j < neighbourRoute.size(); j++) {

                if (verbosity == 'y')
                    cout << neighbourRoute[j] << " ";
            }

            if (verbosity == 'y')
                cout << endl;

            neighbourRoute.insert(neighbourPos, currMove.client);

            if (verbosity == 'y')
                cout << "Rota do vizinho apos ser adicionado: ";

            for (int j = 0; j < neighbourRoute.size(); j++) {

                if (verbosity == 'y')
                    cout << neighbourRoute[j] << " ";
            }

            if (verbosity == 'y')
                cout << endl;

            if (verbosity == 'y')
                cout << "Rota do cliente antes de ser removido: ";

            for (int j = 0; j < clientRoute.size(); j++) {

                if (verbosity == 'y')
                    cout << clientRoute[j] << " ";
            }

            if (verbosity == 'y')
                cout << endl;

            // Remover cliente da sua rota e inserir na rota vizinha
            clientRoute.erase(remove(clientRoute.begin(), clientRoute.end(), currMove.client));

            if (verbosity == 'y')
                cout << "Rota do cliente apos ser removido: ";

            for (int j = 0; j < clientRoute.size(); j++) {

                if (verbosity == 'y')
                    cout << clientRoute[j] << " ";
            }

            if (verbosity == 'y')
                cout << endl;

            if (clientRoute.empty())
                this->numRoutes--;

            actualSol[routeOfClient[currMove.client]] = clientRoute;
            actualSol[routeOfClient[currMove.neighbour]] = neighbourRoute;

            // Computar custo esperado e armazenar a melhor solução encontrada
            double movePenalExpCost = penalizedExpectedLength(actualSol);

            if (clientRoute.empty())
                this->numRoutes++;

            if (movePenalExpCost < bestMoveNotTabuPenalExpCost) {
                bestMoveNotTabuPenalExpCost = movePenalExpCost;
                bestNotTabuRoutes = actualSol;
                bestMoveNotTabu = currMove;
            }
        }
    }

    this->moveDone = bestMoveNotTabu;
    if (this->moveDone.valid) {
        sol.expectedCost = bestMoveNotTabuPenalExpCost;
        sol.routes = bestNotTabuRoutes;
    }
}
