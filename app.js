Algoritmo ILS(InstanceData, K, MIN_PRIZE, MIN_PROB, TIME_LIMIT)

1.  // Etapa de Inicialização
2.  BestSolution ← GerarSoluçãoInicial_Gulosa(InstanceData, K, MIN_PRIZE, MIN_PROB)
3.  BestSolution ← VND(BestSolution)           // busca local inicial

4.  tempoInicial ← TempoAtual()
5.  iter ← 0

6.  Enquanto (TempoAtual() - tempoInicial < TIME_LIMIT):

        // 6.1 Perturbação da Solução Corrente
        disturbed ← DoubleBridge(BestSolution)

        // 6.2 Busca Local Intensiva
        disturbed ← VND(disturbed)

        // 6.3 Avaliação de Confiabilidade
        prob ← AvaliarProbabilidade(disturbed, MIN_PRIZE)

        // 6.4 Critério de Aceitação
        Se (prob ≥ MIN_PROB) E (ObjFunc(disturbed) < ObjFunc(BestSolution)) então
             BestSolution ← disturbed
             iter ← 0
        Fim-Se

7.  Retornar BestSolution