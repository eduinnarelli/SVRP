#include "SVRP.h"

// Impacto aproximado de introdução do cliente b na rota
double TabuSearchSVRP::approxInsertImpact(int a, int b, int c)
{
    return (g.adjMatrix[a][b] + g.adjMatrix[b][c] - g.adjMatrix[a][c]) * g.vertices[b].probOfPresence;
}
