# SVRP - Projeto de Iniciação Científica

**Alunos**: Eduardo Barros Innarelli (170161) e Felipe Lopes de Mello (171119)

**Orientador**: Eduardo Candido Xavier

Neste trabalho estudamos um problema de roteamento de veículos estocástico no qual os clientes e suas demandas são incertos. No problema clássico, o VRP, temos um depósito de onde *m* caminhões devem sair para fazer entregas para *n* clientes. Deve-se construir *m* rotas disjuntas nos clientes iniciando e terminando no depósito, tal que cada cliente seja visitado exatamente uma vez, com o objetivo de minimizar o custo total. Na versão estocástica do problema, alguns clientes são já conhecidos e para um outro conjunto de clientes temos apenas suas probabilidades de ocorrência, o que significa que devem ser atendidos por alguma rota caso sua ocorrência se realize. Nessa versão as demandas dos clientes não são fixas, podendo variar conforme uma distribuição probabilística. Neste caso, o objetivo é computar *m* rotas de custo esperado mínimo tal que a demanda de todos clientes sejam atendidas e a capacidade *C* dos veículos não seja excedida.

# Primeiro Estágio

O objetivo no primeiro estágio é projetar m rotas (uma para cada veículo) de custo esperado mínimo saindo e voltando para o depósito, tal que cada cliente esteja em exatamente uma rota e a demanda esperada de cada rota seja no máximo *C*.

### L-Shaped

Códigos:
 - [Implementação no Gurobi](https://github.com/eduinnarelli/SVRP/blob/master/src/SVRP/first-stage/l-shaped/solve.cpp)
 - [Funções para encontrar componentes conexas em soluções inteiras](https://github.com/eduinnarelli/SVRP/blob/master/src/SVRP/first-stage/l-shaped/utils/connectedComponents.cpp)

Referência:

 - [An Integer L-Shaped Algorithm for the Capacitated Vehicle Routing Problem with Stochastic Demands (Gilbert Laporte, François V. Louveaux, Luc van Hamme)](https://pubsonline.informs.org/doi/pdf/10.1287/opre.50.3.415.7751)

### Heurística Tabu

Códigos:

 - [Fluxo de execução](https://github.com/eduinnarelli/SVRP/blob/master/src/SVRP/first-stage/tabu-search/run.cpp)
 - [Construção de solução e estruturas iniciais](https://github.com/eduinnarelli/SVRP/blob/master/src/SVRP/first-stage/tabu-search/steps/initialize.cpp)
 - [Avaliação de soluções a partir de movimentos entre vizinhos](https://github.com/eduinnarelli/SVRP/blob/master/src/SVRP/first-stage/tabu-search/steps/neighbourhood-search.cpp)
 - [Atualização de estruturas, parâmetros e melhor solução conforme a viabilidade da solução corrente](https://github.com/eduinnarelli/SVRP/blob/master/src/SVRP/first-stage/tabu-search/steps/neighbourhood-search.cpp)
 - [Custos aproximados](https://github.com/eduinnarelli/SVRP/tree/master/src/SVRP/first-stage/tabu-search/utils)

Referência:

 - [A Tabu Search Heuristic for the Vehicle Routing Problem with Stochastic Demands and Customers (Michel Gendreau, Gilbert Laporte and René Séguin)](https://drive.google.com/file/d/1rx7wUG7FoSaIjfLzi5k-S7r-kKIxyNGa/view?usp=sharing)

# Segundo Estágio
No segundo estágio, após serem realizados os eventos aleatórios, são aplicadas as políticas de recurso sob uma rota, que podem incorrer em novos custos. O objetivo é otimizar o custo de transporte esperado, que consiste no custo da viagem mais o custo esperado das políticas de recurso levando em conta todas as possíveis realizações.

### Cálculo do Custo Esperado

Códigos:
 - [Custo esperado total (todas as rotas)](https://github.com/eduinnarelli/SVRP/blob/master/src/SVRP/second-stage/bertsimas-formula/totalExpectedLength.cpp)
 - [Custo esperado de uma rota](https://github.com/eduinnarelli/SVRP/blob/master/src/SVRP/second-stage/bertsimas-formula/routeExpectedLength.cpp) 
 - [Probabilidades usadas](https://github.com/eduinnarelli/SVRP/tree/master/src/SVRP/probabilities)

Referência:

 - [A Vehicle Routing Problem with Stochastic Demands (Bertsimas)](https://www.mit.edu/~dbertsim/papers/Vehicle%20Routing/A%20vehicle%20routing%20problem%20with%20stochastic%20demand.pdf)

## Código Adicional

 - [Estrutura dos Grafos](https://github.com/eduinnarelli/SVRP/blob/master/src/graph.cpp)

# Resultados
 - [L-Shaped e Tabu](https://docs.google.com/spreadsheets/d/1_yRlqPKbVi1mNA_TAB1vVfs2djhf2BRnBR-tUn5ooR8/edit?usp=sharing)
