#include "GreedyAlgorithm.h"
#include <iostream>
#include <algorithm>
#include "../utils/Structs.h"
#include "../evaluate-tour-probability/EvaluateTourProbability.h"

double GreedyAlgorithm::costBenefit (InstanceData data, int i, int j, double C) {
    return data.probability[j];
}

Customers GreedyAlgorithm::kNeighborRandomInsertion(InstanceData data, int K, double C, double minPrize, double minProb) {
    int n = data.size;

    EvaluateTourProbability evaluateTourProb = EvaluateTourProbability();
    Tour feasibleTour; feasibleTour.path.push_back(0); feasibleTour.prize = 0;
    vector<bool> visited(n, false); visited[0] = true;

    while (feasibleTour.path.size() < n)
    {
        double prob = evaluateTourProb.evaluate(
            feasibleTour.path.size(), 
            minPrize, 
            feasibleTour.path, 
            data.probability, 
            data.prize
        );

        //if( prob >= minProb) {
        //    break;
        //}
        
        int last = feasibleTour.path.back();  
        vector<Candidate> candidateList;  

        for (int i = 0; i < n; i++) 
        {
            if (!visited[i]) 
            {
                double attraction = costBenefit(data, last, i, C);
                Candidate newCandidate = {attraction, i};
                candidateList.emplace_back(newCandidate);
            }
        }

        sort(candidateList.begin(), candidateList.end(), 
            [](const Candidate& a, const Candidate& b) {
                return a.attraction > b.attraction; 
            }
        );
        
        if (candidateList.size() > 0) {
            int k = min(K, (int)candidateList.size());
            if (k == 0) break;  // Avoid invalid access
            int randomNumber = rand() % k;
            int nextCustormer = candidateList[randomNumber].id;


            feasibleTour.path.push_back(nextCustormer);
            feasibleTour.cost += data.cost[last][nextCustormer];
            feasibleTour.prize += data.prize[nextCustormer];

            visited[nextCustormer] = true;
        };
    }


    feasibleTour.cost += data.cost[feasibleTour.path.back()][0];
    feasibleTour.path.push_back(0);

    vector<int> notVisited;
    for (int i = 0; i < visited.size(); i++) {
        if (!visited[i]) {
            notVisited.push_back(i);
        }
    }
    
    
    return {feasibleTour, notVisited};
}
