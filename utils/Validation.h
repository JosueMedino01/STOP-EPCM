#ifndef VALIDATION_H
#define VALIDATION_H

#include "Structs.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <set>
#include <sstream>

/**
 * VALIDATION MODULE - Sistema de Validação Integral do Problema STOP
 * 
 * Este módulo valida invariantes críticos do algoritmo ILS+VND para o 
 * Problema de Orienteering em Equipe Estocástico (STOP):
 * 
 * INVARIANTES:
 * 1. Tour deve começar e terminar no depósito (nó 0)
 * 2. Custo do tour = soma das distâncias entre nós consecutivos
 * 3. Prêmio do tour = soma dos prêmios dos nós visitados (exceto nó 0)
 * 4. Nenhum nó duplicado no tour (exceto nó 0 nas extremidades)
 * 5. Cada nó está em EXATAMENTE UM lugar: visitado OU não-visitado
 * 6. Prêmio >= 0 (não pode ser negativo)
 * 7. Custo >= 0 (não pode ser negativo)
 */
class Validation {

public:
    // ============================================================================
    // VALIDAÇÃO PRINCIPAL: Verifica toda a integridade de uma solução completa
    // ============================================================================
    static bool assertSolutionIntegrity(
        const Customers& customers, 
        const InstanceData& data, 
        double MIN_PRIZE,
        const std::string& context = ""
    ) {
        std::string prefix = context.empty() ? "VALIDATION" : context;
        
        try {
            // 1. Valida o tour (custo, prêmio, estrutura)
            assertTourStructure(customers.feasibleTour, data, prefix + " [Tour Structure]");
            assertTourCostCorrectness(customers.feasibleTour, data, prefix + " [Tour Cost]");
            assertTourPrizeCorrectness(customers.feasibleTour, data, prefix + " [Tour Prize]");
            
            // 2. Valida lista de não-visitados
            assertNotVisitedList(customers.notVisited, data.size, prefix + " [NotVisited List]");
            
            // 3. Valida coerência entre tour e não-visitados
            assertPartitionIntegrity(customers.feasibleTour, customers.notVisited, data, 
                                     prefix + " [Partition Integrity]");
            
            return true;
        } 
        catch (const std::exception& e) {
            std::cerr << "❌ " << prefix << " FALHOU: " << e.what() << std::endl;
            return false;
        }
    }

private:
    // ============================================================================
    // VALIDAÇÃO 1: Estrutura do Tour
    // ============================================================================
    static void assertTourStructure(
        const Tour& tour, 
        const InstanceData& data,
        const std::string& context
    ) {
        // 1.1: Tour deve ter pelo menos 2 nós (ida e volta ao depósito)
        assert(tour.path.size() >= 2);
        
        // 1.2: Primeiro nó deve ser 0 (depósito)
        assert(tour.path.front() == 0);
        
        // 1.3: Último nó deve ser 0 (retorno ao depósito)
        assert(tour.path.back() == 0);
        
        // 1.4: Nós devem estar dentro dos limites [0, data.size)
        for (size_t i = 0; i < tour.path.size(); i++) {
            int node = tour.path[i];
            assert(node >= 0 && node < data.size);
        }
        
        // 1.5: Verificar nós duplicados (exceto depósito nas extremidades)
        std::set<int> visitedNodes;
        visitedNodes.insert(0); // depósito pode aparecer no início e fim
        
        for (size_t i = 1; i < tour.path.size() - 1; i++) {
            int node = tour.path[i];
            assert(visitedNodes.find(node) == visitedNodes.end());
            visitedNodes.insert(node);
        }
    }

    // ============================================================================
    // VALIDAÇÃO 2: Custo do Tour
    // ============================================================================
    static void assertTourCostCorrectness(
        const Tour& tour,
        const InstanceData& data,
        const std::string& context
    ) {
        // 2.1: Custo deve ser não-negativo
        assert(tour.cost >= 0);
        
        // 2.2: Recalcular custo do zero (verificação de integridade)
        double calculatedCost = 0.0;
        for (size_t i = 1; i < tour.path.size(); i++) {
            int from = tour.path[i - 1];
            int to = tour.path[i];
            
            assert(from >= 0 && from < data.size);
            assert(to >= 0 && to < data.size);
            
            calculatedCost += data.cost[from][to];
        }
        
        // 2.3: Comparar custo armazenado com custo calculado
        double diff = std::abs(calculatedCost - tour.cost);
        assert(diff <= 1e-6);
    }

    // ============================================================================
    // VALIDAÇÃO 3: Prêmio do Tour
    // ============================================================================
    static void assertTourPrizeCorrectness(
        const Tour& tour,
        const InstanceData& data,
        const std::string& context
    ) {
        // 3.1: Prêmio deve ser não-negativo
        assert(tour.prize >= 0);
        
        // 3.2: Recalcular prêmio do zero
        double calculatedPrize = 0.0;
        for (size_t i = 1; i < tour.path.size() - 1; i++) { // Ignora depósito (nó 0)
            int node = tour.path[i];
            
            assert(node >= 0 && node < (int)data.prize.size());
            
            calculatedPrize += data.prize[node];
        }
        
        // 3.3: Comparar prêmio armazenado com prêmio calculado
        double diff = std::abs(calculatedPrize - tour.prize);
        assert(diff <= 1e-6);
    }

    // ============================================================================
    // VALIDAÇÃO 4: Lista de Não-Visitados
    // ============================================================================
    static void assertNotVisitedList(
        const std::vector<int>& notVisited,
        int dataSize,
        const std::string& context
    ) {
        // 4.1: Todos os nós em notVisited devem estar nos limites
        for (int node : notVisited) {
            assert(node > 0 && node < dataSize);
        }
        
        // 4.2: Nenhum nó duplicado em notVisited
        std::set<int> uniqueNodes;
        for (int node : notVisited) {
            assert(uniqueNodes.find(node) == uniqueNodes.end());
            uniqueNodes.insert(node);
        }
    }

    // ============================================================================
    // VALIDAÇÃO 5: Partição Integral (tour + notVisited = todos os nós)
    // ============================================================================
    static void assertPartitionIntegrity(
        const Tour& tour,
        const std::vector<int>& notVisited,
        const InstanceData& data,
        const std::string& context
    ) {
        // 5.1: Criar conjunto de nós visitados
        std::set<int> visitedInTour;
        for (int node : tour.path) {
            if (node != 0 || tour.path.front() == 0) { // Ignora depósito (exceto primeira/última)
                if (!(node == 0 && (tour.path.size() > 1))) {
                    // Evita adicionar 0 múltiplas vezes
                }
            }
        }
        
        for (size_t i = 1; i < tour.path.size() - 1; i++) {
            visitedInTour.insert(tour.path[i]);
        }
        
        // 5.2: Criar conjunto de nós não-visitados
        std::set<int> notVisitedSet(notVisited.begin(), notVisited.end());
        
        // 5.3: Interseção deve estar vazia
        for (int node : visitedInTour) {
            assert(notVisitedSet.find(node) == notVisitedSet.end());
        }
        
        // 5.4: União deve conter todos os nós (exceto depósito)
        std::set<int> allNodes;
        allNodes.insert(visitedInTour.begin(), visitedInTour.end());
        allNodes.insert(notVisitedSet.begin(), notVisitedSet.end());
        
        for (int i = 1; i < data.size; i++) {
            assert(allNodes.find(i) != allNodes.end());
        }
        
        // 5.5: Tamanho total deve ser data.size - 1 (excluindo depósito)
        int totalNodes = visitedInTour.size() + notVisitedSet.size();
        assert(totalNodes == data.size - 1);
    }

public:
    // ============================================================================
    // MÉTODO AUXILIAR: Imprimir estado de uma solução (debugging)
    // ============================================================================
    static void printSolutionState(
        const Customers& customers,
        const InstanceData& data,
        const std::string& label = "Solution State"
    ) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "Tour Path: ";
        for (int node : customers.feasibleTour.path) {
            std::cout << node << " -> ";
        }
        std::cout << std::endl;
        
        std::cout << "Prêmio: " << customers.feasibleTour.prize << std::endl;
        std::cout << "Custo: " << customers.feasibleTour.cost << std::endl;
        std::cout << "Tour Size: " << customers.feasibleTour.path.size() << std::endl;
        
        std::cout << "Não-Visitados: ";
        for (int node : customers.notVisited) {
            std::cout << node << " ";
        }
        std::cout << std::endl;
        
        std::cout << std::string(60, '=') << std::endl;
    }

    // ============================================================================
    // NOVO ASSERT: Validar Coerência Entre Prize Determinístico e Probabilístico
    // ============================================================================
    // Este assert detecta a incoerência: tour.prize < MIN_PRIZE mas prob > MIN_PROB
    static void assertProbabilityCoherence(
        const Customers& customers,
        const InstanceData& data,
        double MIN_PRIZE,
        double MIN_PROB,
        double calculatedProbability,
        const std::string& context = ""
    ) {
        double deterministicPrize = customers.feasibleTour.prize;
        
        // Criar arquivo de log para coerência
        std::ofstream outFile("solution_log.txt", std::ios::app);
        
        std::string separator(70, '-');
        std::string msg = "\n" + separator + "\n";
        msg += "🔬 VERIFICACAO DE COERENCIA PROBABILISTICA\n";
        msg += "   Contexto: " + context + "\n";
        msg += separator + "\n\n";
        msg += "VALORES CRITICOS:\n";
        msg += "   Prize Determinístico:    " + std::to_string(deterministicPrize) + "\n";
        msg += "   MIN_PRIZE (restricao):   " + std::to_string(MIN_PRIZE) + "\n";
        msg += "   Probabilidade Calculada: " + std::to_string(calculatedProbability * 100) + "%\n";
        msg += "   MIN_PROB (restricao):    " + std::to_string(MIN_PROB * 100) + "%\n\n";
        
        // VERIFICAÇÃO CRÍTICA 1:
        if (deterministicPrize < MIN_PRIZE) {
            msg += "INCOERENCIA CRITICA DETECTADA:\n";
            msg += "   Prize determinístico (" + std::to_string(deterministicPrize) + ") < MIN_PRIZE (" + std::to_string(MIN_PRIZE) + ")\n";
            msg += "   MAS a probabilidade calculada é: " + std::to_string(calculatedProbability * 100) + "%\n\n";
            msg += "ANALISE:\n";
            msg += "   • Se você visitou clientes com prêmio total = " + std::to_string(deterministicPrize) + "\n";
            msg += "   • E precisa de pelo menos " + std::to_string(MIN_PRIZE) + " pontos\n";
            msg += "   • ENTAO a probabilidade de atingir " + std::to_string(MIN_PRIZE) + " é praticamente 0%\n\n";
            msg += "CAUSA PROVAVEL:\n";
            msg += "   • EvaluateTourProbability está incluindo nós NAO-visitados\n";
            msg += "   • Ou está usando semântica incorreta no cálculo recursivo\n";
            msg += "   • Ou há erro no critério de aceitação do ILS\n\n";
            msg += "ALERTA: SOLUCAO NAO DEVERIA TER SIDO ACEITA!\n";
            
            std::cout << msg;
            outFile << msg;
        } 
        else if (deterministicPrize >= MIN_PRIZE) {
            msg += "COERENCIA OK:\n";
            msg += "   Prize determinístico (" + std::to_string(deterministicPrize) + ") >= MIN_PRIZE (" + std::to_string(MIN_PRIZE) + ")\n";
            msg += "   Probabilidade calculada: " + std::to_string(calculatedProbability * 100) + "%\n";
            
            if (calculatedProbability >= MIN_PROB) {
                msg += "   OK: Também satisfaz MIN_PROB\n";
            } else {
                msg += "   AVISO: MAS NAO satisfaz MIN_PROB!\n";
            }
            msg += separator + "\n\n";
            
            outFile << msg;
        }
        
        outFile.close();
        
        // VERIFICAÇÃO CRÍTICA 2: Probabilidade deve estar em [0, 1]
        assert(calculatedProbability >= 0.0 && calculatedProbability <= 1.0);
    }
};

#endif // VALIDATION_H
