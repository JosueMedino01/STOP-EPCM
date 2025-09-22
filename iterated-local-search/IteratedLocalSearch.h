#ifndef ITERATEDLOCALSEARCH_H
#define ITERATEDLOCALSEARCH_H

#pragma once

#include "../utils/Structs.h"
#include "../evaluate-tour-probability/EvaluateTourProbability.h"

class IteratedLocalSearch
{
    private:
        int MAX_NOT_IMPROVIMENT, K, SEED; 
        double MIN_PRIZE, MIN_PROB;
        double objcFunc(double sumCost);
        void printData(Tour tour, vector<int> notVisited, string source);
        EvaluateTourProbability evaluateTourProb;

        bool shiftOneZero(InstanceData &data, Customers &customers);
        bool swapOneOne(InstanceData &data, Customers &customers);
        bool reinsertion(InstanceData &data, Customers &customers);
        bool twoOpt(InstanceData &data, Customers &customers);
        
        void localSearch(InstanceData &data, Customers &customers);
        Customers doubleBridge(InstanceData &data, Customers &customers);
    public:
        Customers bestSolution; 
        IteratedLocalSearch(int MXI, int K, double MIN_PRIZE, double MIN_PROB, int SEED/* , double alpha = 0.6 */);
        
        void run(InstanceData data, int K, double C);
};

#endif