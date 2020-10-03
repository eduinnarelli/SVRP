#include "SVRP.h"

/*
probTotalDemand: Calcula todas as probabilidades de demanda total até cada vértice em uma
rota salvando estes valores em uma matriz. Nesta matriz, as linhas representam o último
vértice sendo considerado e as colunas a possível demanda até este vértice. Assim, se estamos
na 3a linha e 4a coluna, computamos a probabilidade de que a demanda dos vértices 1, 2 e 3
sejam iguais a 4. A ordem dos vértices segue a ordem da rota.

Entrada:
g: grafo do problema sendo considerado;
orderInRoute: vetor de inteiros que indica a ordem na qual os vértices são percorridos na rota.

Saída:
f: matriz n por (20*(n-1))+1, onde n é o número de vértices do grafo.
f[m][r] = probabilidade da demanda total dos clientes 1, ..., m é igual a r.

Observação: demanda máxima de um vértice é 20.
*/
vector<vector<double> > probTotalDemand(Graph g, vector<int> route)
{

    // Inicializa a matriz com probabilidades 0
    int next, orderInRoute = 1, routeSize = route.size();
    vector<double> v(20 * routeSize + 1, 0);
    vector<vector<double> > f(route.size() + 1, v);
    double probDemandK;

    f[0][0] = 1; // probabilidade da carga do veiculo até o depósito ser 0

    while (orderInRoute <= routeSize) {

        // Considera o próximo cliente da rota
        next = route[orderInRoute - 1];

        // Probabilidade de não haver nenhuma carga até o cliente, ou seja, todos ausentes
        f[orderInRoute][0] = (1 - g.vertices[next].probOfPresence) * f[orderInRoute - 1][0];

        // Para todas as demandas até o cliente possíveis
        for (int dem = 1; dem <= 20 * orderInRoute; dem++) {

            // Probabilidade do cliente estar ausente, mantendo a mesma demanda anterior
            f[orderInRoute][dem] += (1 - g.vertices[next].probOfPresence) * f[orderInRoute - 1][dem];

            // Para todas as demandas possíveis do cliente
            for (int k = 1; k <= min(20, dem); k++) {

                // Probabilidade do cliente estar presente com uma demanda k
                probDemandK = g.vertices[next].probOfPresence * g.vertices[next].probDemand[k];

                if (probDemandK > 0) {
                    f[orderInRoute][dem] += probDemandK * f[orderInRoute - 1][dem - k];
                }
            }
        }
        orderInRoute++;
    }

    return f;
}
