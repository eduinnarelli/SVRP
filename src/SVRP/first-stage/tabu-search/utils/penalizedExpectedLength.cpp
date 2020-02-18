#include "SVRP.h"

// Função objetivo com penalização de soluções inviáveis
double TabuSearchSVRP::penalizedExpectedLength(vector<vector<int>> sol) {

		double aux = Bertsimas2ndStage::totalExpectedLength(g, capacity, sol) + 
            penalty*abs(this->numRoutes - numVehicles);

		if(verbosity=='y')
			cout << "Custo penalizado total: " << aux << endl;

    return aux;
}
