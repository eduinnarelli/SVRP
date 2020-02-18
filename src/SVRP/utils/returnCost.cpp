#include "SVRP.h"

/*
returnCost: Calcula o custo de ir ao depósito a partir do cliente i e retornar ao cliente j.
i e j são as posições dos clientes na rota orderInRoute;
*/
double returnCost(int i, int j, Graph g, vector<int> orderInRoute) {
	return 
        g.adjMatrix[orderInRoute[i]][0] + 
        g.adjMatrix[0][orderInRoute[j]] - 
        g.adjMatrix[orderInRoute[i]][orderInRoute[j]];
}
