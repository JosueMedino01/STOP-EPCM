#include "IteratedLocalSearch.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include "../greedy-algorithm/GreedyAlgorithm.h"
#include "../evaluate-tour-probability/EvaluateTourProbability.h"
#include "../utils/Validation.h"

IteratedLocalSearch::IteratedLocalSearch(int MXI, int K, double MIN_PRIZE, double MIN_PROB, int SEED, int TIME_LIMIT)
{
    this->MAX_NOT_IMPROVIMENT = MXI;
    this->K = K;
    this->MIN_PRIZE = MIN_PRIZE;
    this->MIN_PROB = MIN_PROB;
    this->SEED = SEED;
    this->TIME_LIMIT = TIME_LIMIT;
    srand(SEED); 
    this->evaluateTourProb = EvaluateTourProbability();
}


void IteratedLocalSearch::run(InstanceData data, int K, double C) 
{
    /* Melhor Solução - Solução Inicial*/
    GreedyAlgorithm greedy; 
    this->bestSolution = greedy.kNeighborRandomInsertion(data, K, C, this->MIN_PRIZE, this->MIN_PROB);
    
    // ASSERT: Valida solução inicial
    if (!Validation::assertSolutionIntegrity(bestSolution, data, MIN_PRIZE, "SOLUCAO INICIAL")) {
        std::cerr << "ERRO: Solução inicial inválida!" << std::endl;
        exit(1);
    }
    
    this->printData(bestSolution.feasibleTour, bestSolution.notVisited, "SOLUCAO INICIAL", data);
    this->localSearch(data, bestSolution); 
    
    // ASSERT: Valida solução após busca local inicial
    if (!Validation::assertSolutionIntegrity(bestSolution, data, MIN_PRIZE, "APOS BUSCA LOCAL INICIAL")) {
        std::cerr << "ERRO: Solução após busca local inicial inválida!" << std::endl;
        exit(1);
    } 

    auto start = std::chrono::steady_clock::now();
    int i = 0;
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() < TIME_LIMIT)
    {
        i++;
        // out << "Iteracao: " << i << endl; 
        /* Pertubacao */
        Customers disturbed =  doubleBridge(data, bestSolution);
        
        // ASSERT: Valida solução perturbada
        if (!Validation::assertSolutionIntegrity(disturbed, data, MIN_PRIZE, "APOS PERTURBACAO")) {
            std::cerr << "ERRO: Solução perturbada inválida!" << std::endl;
            exit(1);
        }
        
        this->printData(disturbed.feasibleTour, disturbed.notVisited, "Perturbacao", data);
        
        if(disturbed.feasibleTour.cost < 0) {
            cout << "doubleBridge - Custo negativo encontrado, abortando..." << endl;
            exit(1);
        }

        /* Busca Local */
        this->localSearch(data, disturbed);
        
        // ASSERT: Valida solução após busca local
        if (!Validation::assertSolutionIntegrity(disturbed, data, MIN_PRIZE, "APOS BUSCA LOCAL")) {
            std::cerr << "ERRO: Solução após busca local inválida!" << std::endl;
            exit(1);
        }
        
        this->printData(disturbed.feasibleTour, disturbed.notVisited, "Local search", data);
        

        double prob = this->evaluateTourProb.evaluate(
            disturbed.feasibleTour.path.size() - 1, 
            MIN_PRIZE, 
            disturbed.feasibleTour.path, 
            data.probability, 
            data.prize
        );

        double bestProb = this->evaluateTourProb.evaluate(
            this->bestSolution.feasibleTour.path.size() - 1, 
            MIN_PRIZE, 
            this->bestSolution.feasibleTour.path, 
            data.probability, 
            data.prize
        );

        // cout << "Probabilidade: " << prob << endl;
        // cout << "Min Prob: " << MIN_PROB << endl;

        /* Criterio de Aceitação */
        // CORRECAO: Rejeitar se prize < MIN_PRIZE (determinístico)
        bool prizeConstraintMet = (disturbed.feasibleTour.prize >= MIN_PRIZE);
        bool probConstraintMet = (prob > MIN_PROB);
        bool costConstraintMet = this->objcFunc(disturbed.feasibleTour.cost) < this->objcFunc(bestSolution.feasibleTour.cost);
        
        if (prizeConstraintMet && probConstraintMet && costConstraintMet) {
            // NOVO ASSERT: Verifica coerência probabilística ANTES de aceitar
            Validation::assertProbabilityCoherence(
                disturbed, data, MIN_PRIZE, MIN_PROB, prob, 
                "PRE-ACEITACAO: Verificando se solução é coerente"
            );
            
            this->bestSolution = disturbed;
            
            // ASSERT: Valida atualização da melhor solução
            if (!Validation::assertSolutionIntegrity(bestSolution, data, MIN_PRIZE, "APOS ACEITAR NOVA MELHOR SOLUCAO")) {
                std::cerr << "ERRO: Melhor solução inválida após atualização!" << std::endl;
                exit(1);
            }
            
            cout << "HOUVE MELHORA NA ITERACAO "<< i <<endl;
            i = 0;
        }
    }
    
    // ASSERT: Valida solução final
    if (!Validation::assertSolutionIntegrity(bestSolution, data, MIN_PRIZE, "SOLUCAO FINAL")) {
        std::cerr << "ERRO: Solução final inválida!" << std::endl;
        exit(1);
    }
    
    this->printData(bestSolution.feasibleTour, bestSolution.notVisited, "Solucao Final", data);
}

double IteratedLocalSearch::objcFunc(double sumCost) {
    return sumCost;
}

// remove um cliente visitado do tour se a probabilidade mínima ainda for satisfeita
bool IteratedLocalSearch::removeVisitedIfSafe(InstanceData &data, Customers &customers) {
    // Não tente remover se o tour é muito pequeno (apenas origem e destino)
    if (customers.feasibleTour.path.size() <= 3) return false;

    int n = customers.feasibleTour.path.size();
    double bestCost = customers.feasibleTour.cost;
    int bestRemoveIndex = -1;
    double bestNewCost = bestCost;
    double bestNewPrize = customers.feasibleTour.prize;

    // Testa remover cada vértice interior (i = 1 .. n-2)
    for (int i = 1; i < n - 1; ++i) {
        int removedNode = customers.feasibleTour.path[i];

        // Calculo custo se remover o nó i
        double removedEdges = data.cost[customers.feasibleTour.path[i - 1]][removedNode]
                            + data.cost[removedNode][customers.feasibleTour.path[i + 1]];
        double addedEdge   = data.cost[customers.feasibleTour.path[i - 1]][customers.feasibleTour.path[i + 1]];
        double newCost = customers.feasibleTour.cost - removedEdges + addedEdge;

        // Novo prêmio se remover esse cliente
        double newPrize = customers.feasibleTour.prize - data.prize[removedNode];

        if (newCost >= bestNewCost) {
            // não melhora custo global, ignora
            continue;
        }

        if (newCost < 0) {
            std::cout << "removeVisitedIfSafe - Custo negativo encontrado, abortando..." << std::endl;
            exit(1);
        }

        // Monta o novo path (removido o nó i) para avaliar probabilidade
        std::vector<int> newPath;
        newPath.reserve(n - 1);
        for (int k = 0; k < n; ++k) {
            if (k == i) continue;
            newPath.push_back(customers.feasibleTour.path[k]);
        }

        // A função evaluate espera path.size() - 1 como primeiro parâmetro (nó de retorno ao depósito)
        double prob = this->evaluateTourProb.evaluate(
            (int)newPath.size() - 1,
            MIN_PRIZE,
            newPath,
            data.probability,
            data.prize
        );

        // Se a probabilidade ainda satisfaz MIN_PROB E o prêmio >= MIN_PRIZE, podemos considerar a remoção
        // CORREÇÃO: Garantir que a solução satisfaz AMBAS as constraints
        if (prob >= MIN_PROB && newPrize >= MIN_PRIZE) {
            // guarda melhor remoção encontrada (maior redução de custo)
            bestRemoveIndex = i;
            bestNewCost = newCost;
            bestNewPrize = newPrize;
            // Observação: podemos parar no primeiro (first-improvement) ou buscar o melhor (best-improvement)
            // Aqui escolhi best-improvement: continuamos a busca por remoção ainda melhor.
        }
    }

    // Se encontramos uma remoção válida que melhora custo, executa-a
    if (bestRemoveIndex != -1) {
        int removedNode = customers.feasibleTour.path[bestRemoveIndex];
        // remove do caminho
        customers.feasibleTour.path.erase(customers.feasibleTour.path.begin() + bestRemoveIndex);
        // atualiza custo e prêmio
        customers.feasibleTour.cost = bestNewCost;
        customers.feasibleTour.prize = bestNewPrize;
        // adiciona o nó removido à lista de não visitados
        customers.notVisited.push_back(removedNode);

        // ASSERT: Valida estado após remoção
        if (!Validation::assertSolutionIntegrity(customers, data, MIN_PRIZE, "removeVisitedIfSafe")) {
            std::cerr << "ERRO: Solução inválida após removeVisitedIfSafe!" << std::endl;
            exit(1);
        }

        // segurança: se custo negativo, aborta (seguindo padrão do código)
        if (customers.feasibleTour.cost < 0) {
            std::cout << "removeVisitedIfSafe - Custo negativo encontrado apos remocao, abortando..." << std::endl;
            exit(1);
        }

        return true;
    }

    return false;
}

/* Move um cliente de não visitado para o caminho viável */
bool IteratedLocalSearch::shiftOneZero(InstanceData &data, Customers &customers) {
    int n = customers.feasibleTour.path.size();
    int bestNotVisitedIndex = -1, bestPositionIndex = -1;
    double bestCost = customers.feasibleTour.cost, bestPrize = customers.feasibleTour.prize; 

    for(int i = 1; i < n - 1; i++) {
        for (int j = 0; j < customers.notVisited.size(); j++)
        {   
            int addedNode = customers.notVisited[j];

            double dellEdges = data.cost[customers.feasibleTour.path[i - 1]][customers.feasibleTour.path[i]];
            double newEdges = data.cost[customers.feasibleTour.path[i - 1]][addedNode] 
                + data.cost[addedNode][customers.feasibleTour.path[i]]; 
            
            double newCost = customers.feasibleTour.cost - dellEdges + newEdges;
            double newPrize = customers.feasibleTour.prize + data.prize[addedNode];

            double a = this->objcFunc(newCost);
            double b = this->objcFunc(bestCost);

            if( a < b ) {
                bestPositionIndex = i;
                bestNotVisitedIndex = j;
                bestCost = newCost;
                bestPrize = newPrize;
                //cout << "HOUVE MELHORA - shiftOneZero " << a << " < "<< b << endl;
            }
        }
    }

    if(bestCost< 0) {
        cout << "shiftOneZero - Custo negativo encontrado, abortando..." << endl;
        exit(1);
    }

    if (bestNotVisitedIndex != -1 && bestPositionIndex != -1) {
        int addedNode = customers.notVisited[bestNotVisitedIndex];
        customers.notVisited.erase(customers.notVisited.begin() + bestNotVisitedIndex);
        customers.feasibleTour.path.insert(customers.feasibleTour.path.begin() + bestPositionIndex, addedNode);

        customers.feasibleTour.cost = bestCost;
        customers.feasibleTour.prize = bestPrize;
        
        // ASSERT: Valida estado após inserção
        if (!Validation::assertSolutionIntegrity(customers, data, MIN_PRIZE, "shiftOneZero")) {
            std::cerr << "ERRO: Solução inválida após shiftOneZero!" << std::endl;
            exit(1);
        }
        
        return true;
    }

    return false;
}

/* Troca entre um cliente do tour e um cliente não visitado */
bool IteratedLocalSearch::swapOneOne(InstanceData &data, Customers &customers) {
    int n = customers.feasibleTour.path.size();
    int bestNotVisitedIndex = -1, bestPositionIndex = -1;
    double bestCost = customers.feasibleTour.cost, bestPrize = customers.feasibleTour.prize; 

    for(int i = 1; i < n - 1; i++) {
        for (int j = 0; j < customers.notVisited.size(); j++)
        {   
            int addedNode = customers.notVisited[j];
            int deletedNode = customers.feasibleTour.path[i];

            double dellEdges = data.cost[customers.feasibleTour.path[i - 1]][deletedNode] 
                + data.cost[deletedNode][customers.feasibleTour.path[i + 1]];

            double newEdges = data.cost[customers.feasibleTour.path[i - 1]][addedNode] 
                + data.cost[addedNode][customers.feasibleTour.path[i + 1]]; 
            
            double newCost = customers.feasibleTour.cost - dellEdges + newEdges;
            double newPrize = customers.feasibleTour.prize + data.prize[addedNode] - data.prize[deletedNode];

            if( this->objcFunc(newCost) < this->objcFunc(bestCost)) {
                bestPositionIndex = i;
                bestNotVisitedIndex = j;
                bestCost = newCost;
                bestPrize = newPrize;
            }
        }
    }

    if(bestCost< 0) {
        cout << "swapOneOne - Custo negativo encontrado, abortando..." << endl;
        exit(1);
    }

    if (bestNotVisitedIndex != -1 && bestPositionIndex != -1) {
        int addedNode = customers.notVisited[bestNotVisitedIndex];
        int deletedNode = customers.feasibleTour.path[bestPositionIndex];

        customers.notVisited[bestNotVisitedIndex] = deletedNode;
        customers.feasibleTour.path[bestPositionIndex] = addedNode;

        customers.feasibleTour.cost = bestCost;
        customers.feasibleTour.prize = bestPrize;
        
        // ASSERT: Valida estado após swap
        if (!Validation::assertSolutionIntegrity(customers, data, MIN_PRIZE, "swapOneOne")) {
            std::cerr << "ERRO: Solução inválida após swapOneOne!" << std::endl;
            exit(1);
        }
        
        return true;
    }

    return false;
}

bool IteratedLocalSearch::reinsertion(InstanceData &data, Customers &customers) {
    if (customers.feasibleTour.path.size() < 3) return false;

    int n = customers.feasibleTour.path.size();
    int bestRemovedIndex = -1, bestInsertionIndex = -1;
    double bestCost = customers.feasibleTour.cost; 

    for(int i = n - 2; i >= 2; i--) {
        int deletedNode = customers.feasibleTour.path[i];

        for (int j = 1; j < i; j++)
        {   
            if(i != j && i != j+1) {
                double dellEdges = data.cost[customers.feasibleTour.path[i - 1]][deletedNode] 
                    + data.cost[deletedNode][customers.feasibleTour.path[i + 1]]
                    + data.cost[customers.feasibleTour.path[j - 1]][customers.feasibleTour.path[j]];

                double newEdges = data.cost[customers.feasibleTour.path[j - 1]][customers.feasibleTour.path[i]] 
                    + data.cost[customers.feasibleTour.path[i]][customers.feasibleTour.path[j]] 
                    + data.cost[customers.feasibleTour.path[i-1]][customers.feasibleTour.path[i+1]]; 
                
                double newCost = customers.feasibleTour.cost - dellEdges + newEdges;

                if( (newCost <= bestCost)) {
                    bestRemovedIndex = i;
                    bestInsertionIndex = j;
                    bestCost = newCost;
                    
                }
            }
        }
    }
    if(bestCost < 0) {
        cout << "reinsertion - Custo negativo encontrado, abortando..." << endl;
        exit(1);
    }

    if (bestRemovedIndex != -1 && bestInsertionIndex != -1) {
        int bestRemovedNode = customers.feasibleTour.path[bestRemovedIndex];
        
        customers.feasibleTour.path.erase(customers.feasibleTour.path.begin() + bestRemovedIndex);

        if (bestRemovedIndex < bestInsertionIndex) {
            customers.feasibleTour.path.insert(customers.feasibleTour.path.begin() + (bestInsertionIndex - 1), bestRemovedNode);
        } else {
            customers.feasibleTour.path.insert(customers.feasibleTour.path.begin() + bestInsertionIndex, bestRemovedNode);
        }
        
        customers.feasibleTour.cost = bestCost;
        
        // ASSERT: Valida estado após reinserção
        if (!Validation::assertSolutionIntegrity(customers, data, MIN_PRIZE, "reinsertion")) {
            std::cerr << "ERRO: Solução inválida após reinsertion!" << std::endl;
            exit(1);
        }
        
        return true;
    }

    return false;
}

bool IteratedLocalSearch::twoOpt(InstanceData &data, Customers &customers) {
    int n = customers.feasibleTour.path.size();
    double bestCost = customers.feasibleTour.cost;
    int bestIndexI = -1, bestIndexJ = -1;

    for (int i = 1  ; i < n; i++) {
        for (int j = i + 1; j < n - 1; j++) {
            double edgeRemoved  =  data.cost[customers.feasibleTour.path[i - 1]][customers.feasibleTour.path[i]] + data.cost[customers.feasibleTour.path[j]][customers.feasibleTour.path[j + 1]];
            double edgeAdded = data.cost[customers.feasibleTour.path[i - 1]][customers.feasibleTour.path[j]] + data.cost[customers.feasibleTour.path[i]][customers.feasibleTour.path[j + 1]];
            double newCost = customers.feasibleTour.cost - edgeRemoved + edgeAdded;

            if (newCost < bestCost)
            {
                bestCost = newCost;
                bestIndexI = i;
                bestIndexJ = j;
            }
        }
    }

    if(bestCost < 0) {
        cout << "twoOpt - Custo negativo encontrado, abortando..." << endl;
        exit(1);
    }
    
    if (bestIndexI != -1 && bestIndexJ != -1) {
        reverse(customers.feasibleTour.path.begin() + bestIndexI, customers.feasibleTour.path.begin() + bestIndexJ+ 1);
        customers.feasibleTour.cost = bestCost;
        
        // ASSERT: Valida estado após 2-opt
        if (!Validation::assertSolutionIntegrity(customers, data, MIN_PRIZE, "twoOpt")) {
            std::cerr << "ERRO: Solução inválida após twoOpt!" << std::endl;
            exit(1);
        }
        
        return true;
    }

    return false;
}


void  IteratedLocalSearch::localSearch(InstanceData &data, Customers &customers) {
    int r = 5, k = 1;

    while(k <= r) {
        bool hasImprovement = false;
       
        switch(k) {
            case 1:
                hasImprovement = this->shiftOneZero(data, customers);
                break;
            case 2:
                hasImprovement = this->swapOneOne(data, customers);
                break;
            case 3: 
                hasImprovement = this->reinsertion(data, customers);
                break;
            case 4: 
                hasImprovement = this->twoOpt(data, customers);
                break;
            case 5:
                hasImprovement = this->removeVisitedIfSafe(data, customers);
                break;
        }

        k = hasImprovement ? 1 : k + 1;
    }
}
/* https://iopscience.iop.org/article/10.1088/1742-6596/1320/1/012025/pdf */


Customers IteratedLocalSearch::doubleBridge(InstanceData &data, Customers &customersInput) {
    Customers customers = customersInput;
    int n_with_depot  = customers.feasibleTour.path.size() - 1;

    if (n_with_depot < 8) return customers;

    customers.feasibleTour.path.pop_back(); 
    customers.feasibleTour.cost -= data.cost[customers.feasibleTour.path.back()][0];
    
    int n = customers.feasibleTour.path.size();
    
    int partSize = max(1, (n - 4) / 4);
    
    int cuts[4];
    cuts[0] = 1 + rand() % partSize;
    cuts[1] = cuts[0] + 1 + rand() % partSize;
    cuts[2] = cuts[1] + 1 + rand() % partSize;
    cuts[3] = cuts[2] + 1 + rand() % (n - cuts[2] - 2);


    vector<int> newTour;
    newTour.insert(newTour.end(), customers.feasibleTour.path.begin(), customers.feasibleTour.path.begin() + cuts[0]);
    newTour.insert(newTour.end(), customers.feasibleTour.path.begin() + cuts[3], customers.feasibleTour.path.end());
    newTour.insert(newTour.end(), customers.feasibleTour.path.begin() + cuts[2], customers.feasibleTour.path.begin() + cuts[3]);
    newTour.insert(newTour.end(), customers.feasibleTour.path.begin() + cuts[1], customers.feasibleTour.path.begin() + cuts[2]);
    newTour.insert(newTour.end(), customers.feasibleTour.path.begin() + cuts[0], customers.feasibleTour.path.begin() + cuts[1]);

    // Custo -> forma errada, mas provisória
    double newCost = 0;

    for (int i = 1; i < newTour.size(); i++) {
        newCost += data.cost[newTour[i-1]][newTour[i]];
    }


    customers.feasibleTour.path = newTour;
    customers.feasibleTour.cost = newCost;
    

    customers.feasibleTour.cost += data.cost[customers.feasibleTour.path.back()][0];
    customers.feasibleTour.path.push_back(0);
    
    // ASSERT: Valida estado após doubleBridge
    // Nota: Prize não é alterado no doubleBridge, apenas o path é reorganizado
    if (!Validation::assertSolutionIntegrity({customers.feasibleTour, customers.notVisited}, data, MIN_PRIZE, "doubleBridge")) {
        std::cerr << "ERRO: Solução inválida após doubleBridge!" << std::endl;
        exit(1);
    }

    return {customers.feasibleTour, customers.notVisited};
}

void IteratedLocalSearch::printData(Tour tour, vector<int> notVisited, string source, InstanceData &data) {
    ofstream outFile("solution_log.txt", ios::app); // Abre o arquivo em modo append

    if (!outFile) {
        cerr << "Erro ao abrir o arquivo para escrita!" << endl;
        return;
    }
    outFile << "\nOrigem: " << source;
    outFile << "\nTour: ";
    for (int i = 0; i < tour.path.size(); i++) {
        // CORRECAO: Converter índices 0-indexed para IDs 1-indexed (exceto depot que permanece 0)
        if (tour.path[i] == 0) {
            outFile << "0 ";
        } else {
            outFile << (tour.path[i] + 1) << " ";  // +1 para converter de índice para ID
        }
    }
    
    outFile << "\nNot visited: ";
    for (int i = 0; i < notVisited.size(); i++) {
        // CORRECAO: Converter para IDs 1-indexed
        outFile << (notVisited[i] + 1) << " ";
    }

    outFile << "\nCost: " << tour.cost;
    outFile << "\nPrize: " << tour.prize;

    // Adicionando informações de probabilidade
    double prob = this->evaluateTourProb.evaluate(
        tour.path.size() - 1, 
        MIN_PRIZE, 
        tour.path, 
        data.probability, 
        data.prize
    );
    outFile << "\nProbabilidade: " << prob << endl;
    outFile << "Min Prob: " << MIN_PROB << endl;

    outFile.close(); // Fecha o arquivo corretamente
}