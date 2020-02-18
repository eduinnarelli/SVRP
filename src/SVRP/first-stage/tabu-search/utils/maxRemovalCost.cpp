#include "SVRP.h"

/* Maior custo de remover um cliente da rota r, usado como aproximação
do custo de inserção de um cliente. */
double TabuSearchSVRP::maxRemovalCost(int r) {

	double max = 0, aux = 0;

	for(int i = 0; i < sol.routes[r].size(); i++) {

		aux = removalCost(r, sol.routes[r][i]);

		if(aux > max)
			max = aux;

	}

	return max;

}
