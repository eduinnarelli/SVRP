#include "SVRP.h"

/*
probExceedsCapacity: Calcula a probabilidade da demanda até o vértice 'i' do grafo 'g' ser
maior do que a capacidade 'capacity'. Utiliza as probabilidades calculadas na função
probTotalDemand, a diferença é que esta função considera todos os números possíveis
de falhas, o que é equivalente a aumentar a capacidade.

Entrada:
i: vértice cuja probabilidade de demanda até ele é calculado;
g: grafo do problema sendo considerado;
f: matriz n por (20*(n-1))+1, onde n é o número de vértices do grafo.
f[m][r] = probabilidade da demanda total dos clientes 1, ..., m é igual a r.
capacity: capacidade máxima do veículo do problema.
orderInRoute: ordem dos vértices na rota. orderInRoute[2] = 3o cliente da rota.

Saída: double indicando a probabilidade da demanda até o vértice 'i' ser maior do que a
capacidade máxima do veículo.
*/
double probExceedsCapacity(int i, Graph g, vector<vector<double> > f, int capacity, vector<int> orderInRoute)
{

    double probExceedsCap = 0, probDemandExceeds = 0;
    int vtx = orderInRoute[i];

    // Para todos os possíveis números de falhas "q"
    for (int q = 1; q <= floor((i + 1) * 20 / capacity); q++) {

        // Para todas as possíveis "k" capacidades residuais no vértice anterior a "i"
        for (int k = 1; k <= 19; k++) {

            // Para todas as possíveis demandas "r" do vértice "i" maiores do que "k"
            for (int r = k + 1; r <= 20; r++) {
                probDemandExceeds += g.vertices[vtx].probOfPresence * g.vertices[vtx].probDemand[r];
            }

            /* Somar probabilidade de que a demanda em "i" é maior do que "k"
						e que a demanda anterior a "i" seja igual a q*capacity-k */
            probExceedsCap += probDemandExceeds * f[i][q * capacity - k];
            probDemandExceeds = 0;
        }
    }

    return probExceedsCap;
}
