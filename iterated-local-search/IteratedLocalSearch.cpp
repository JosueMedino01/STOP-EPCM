#include "IteratedLocalSearch.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "../greedy-algorithm/GreedyAlgorithm.h"
#include "../evaluate-tour-probability/EvaluateTourProbability.h"

double alfa = 0.8; // Valor padrão para alpha, pode ser ajustado conforme necessário
double Pmin = 10000;

IteratedLocalSearch::IteratedLocalSearch(int MXI, int K, double C, int SEED)
{
    this->MAX_NOT_IMPROVIMENT = MXI;
    this->K = K;
    this->C = C;
    this->SEED = SEED;
    srand(SEED); 
    this->evaluateTourProb = EvaluateTourProbability();
}


void IteratedLocalSearch::run(InstanceData data, int K, double C) 
{
    /* Melhor Solução - Solução Inicial*/
    GreedyAlgorithm greedy; 
    this->bestSolution = greedy.kNeighborRandomInsertion(data, K, C); 
    this->printData(bestSolution.feasibleTour, bestSolution.notVisited, "SOLUCAO INICIAL");
    this->localSearch(data, bestSolution); 

    int i = 0;
    while (i < this-> MAX_NOT_IMPROVIMENT)
    {
        i++;
        /* cout << "Iteracao: " << i << endl; */
        /* Pertubacao */
        Customers disturbed =  doubleBridge(data, bestSolution);
        printData(disturbed.feasibleTour, disturbed.notVisited, "Perturbacao");
        
        if(disturbed.feasibleTour.cost < 0) {
            cout << "doubleBridge - Custo negativo encontrado, abortando..." << endl;
            exit(1);
        }

        /* Busca Local */
        this->localSearch(data, disturbed); 

        double probPertubed = this->evaluateTourProb.evaluate(
            disturbed.feasibleTour.path.size() - 1,
            Pmin,
            disturbed.feasibleTour.path,
            data.probability,
            data.prize
        );

        double probBest = this->evaluateTourProb.evaluate(
            bestSolution.feasibleTour.path.size() - 1,
            Pmin,
            bestSolution.feasibleTour.path,
            data.probability,
            data.prize
        );

        /* Criterio de Aceitação */
        if (
            this->objcFunc(disturbed.feasibleTour.prize, disturbed.feasibleTour.cost, probPertubed) <
            this->objcFunc(bestSolution.feasibleTour.prize, bestSolution.feasibleTour.cost, probBest)
        ) {
            this->bestSolution = disturbed;
            cout << "HOUVE MELHORA NA ITERACAO "<< i <<endl;
            i = 0;
        }
        
    }
    
    this->printData(bestSolution.feasibleTour, bestSolution.notVisited, "Solucao Final");
}

double IteratedLocalSearch::objcFunc(double sumPrize, double sumCost, double prob) {
    double delta = max(0.0, alfa - prob); // Penalidade só se prob < alfa
    return sumCost * (1 + delta * 10);
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

            /* Caminho temporário */
            vector<int> tempPath = customers.feasibleTour.path;
            tempPath.insert(tempPath.begin() + i, addedNode);

            double prob = this->evaluateTourProb.evaluate(
                tempPath.size() - 1,
                Pmin,
                tempPath,
                data.probability,
                data.prize
            );


            /* cout << "Probabilidade: " << prob << endl; */
            
            double probBest = this->evaluateTourProb.evaluate(
                customers.feasibleTour.path.size() - 1,
                Pmin,
                customers.feasibleTour.path,
                data.probability,
                data.prize
            );

            double a = this->objcFunc(newPrize, newCost, prob);
            double b = this->objcFunc(bestPrize, bestCost, probBest);
            if( newCost <= data.deadline  && a < b ) {
                bestPositionIndex = i;
                bestNotVisitedIndex = j;
                bestCost = newCost;
                bestPrize = newPrize;
                cout << "HOUVE MELHORA - shiftOneZero " << a << " < "<< b << endl;
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

            /* Caminho temporário */
            vector<int> tempPath = customers.feasibleTour.path; 
            tempPath[i] = addedNode;

            double prob = this->evaluateTourProb.evaluate(
                tempPath.size() - 1,
                Pmin,
                tempPath,
                data.probability,
                data.prize
            );

            double probBest = this->evaluateTourProb.evaluate(
                customers.feasibleTour.path.size() - 1,
                Pmin,
                customers.feasibleTour.path,
                data.probability,
                data.prize
            );


            if( newCost <= data.deadline  && this->objcFunc(newPrize, newCost, prob) < this->objcFunc(bestPrize, bestCost, probBest)) {
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

                if( /* newCost <= data.deadline &&  */ (newCost <= bestCost)) {
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
        return true;
    }

    return false;
}


void  IteratedLocalSearch::localSearch(InstanceData &data, Customers &customers) {
    int r = 4, k = 1;

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

    // Verificar deadline e remove clientes se necessário
    while (customers.feasibleTour.cost + data.cost[customers.feasibleTour.path.back()][0] > data.deadline && customers.feasibleTour.path.size() > 2) {
        int last = customers.feasibleTour.path.back();
        customers.feasibleTour.path.pop_back();
        customers.feasibleTour.cost -= data.cost[customers.feasibleTour.path.back()][last];
        if(last != 0) {
            customers.feasibleTour.prize -= data.prize[last];
            customers.notVisited.push_back(last);
        }
    }

    customers.feasibleTour.cost += data.cost[customers.feasibleTour.path.back()][0];
    customers.feasibleTour.path.push_back(0); 

    return {customers.feasibleTour, customers.notVisited};
}

void IteratedLocalSearch::printData(Tour tour, vector<int> notVisited, string source) {
    ofstream outFile("solution_log.txt", ios::app); // Abre o arquivo em modo append

    if (!outFile) {
        cerr << "Erro ao abrir o arquivo para escrita!" << endl;
        return;
    }
    outFile << "\nOrigem: " << source;
    outFile << "\nTour: ";
    for (int i = 0; i < tour.path.size(); i++) {
        outFile << tour.path[i] << " ";
    }
    
    outFile << "\nNot visited: ";
    for (int i = 0; i < notVisited.size(); i++) {
        outFile << notVisited[i] << " ";
    }

    outFile << "\nCost: " << tour.cost;
    outFile << "\nPrize: " << tour.prize;
 

    outFile.close(); // Fecha o arquivo corretamente
}