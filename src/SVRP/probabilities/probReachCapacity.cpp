#include "SVRP.h"

/*
probReachCapacity: Calcula a probabilidade da demanda até o vértice 'i' do grafo 'g' ser
exatamente igual a capacidade 'capacity'. Utiliza as probabilidades calculadas na função
probTotalDemand, a diferença é que esta função considera todos os números de possíveis
falhas, o que é equivalente a aumentar a capacidade.

Entrada:
i: vértice cuja probabilidade de demanda até ele é calculado;
g: grafo do problema sendo considerado;
f: matriz n por (20*(n-1))+1, onde n é o número de vértices do grafo.
f[m][r] = probabilidade da demanda total dos clientes 1, ..., m é igual a r.
capacity: capacidade máxima do veículo do problema.
orderInRoute: ordem dos vértices na rota. orderInRoute[2] = 3o cliente da rota.

Saída: double indicando a probabilidade da demanda até o vértice 'i' ser exatamente igual a
capacidade máxima do veículo.
*/
double probReachCapacity(int i, Graph g, vector<vector<double> > f, int capacity, vector<int> orderInRoute)
{

    double probReachCap = 0;
    int vtx = orderInRoute[i];

    // Para todos os possíveis números de falhas "q"
    for (int q = 1; q <= floor((i + 1) * 20 / capacity); q++) {

        // Para todas as possíveis "k" capacidades residuais no vértice anterior a "i"
        for (int k = 1; k <= 20; k++) {
            probReachCap += g.vertices[vtx].probOfPresence * g.vertices[vtx].probDemand[k] * f[i][q * capacity - k];
        }
    }

    return probReachCap;
}
