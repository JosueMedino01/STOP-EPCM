#include "../../package.h"
#include "../structures/instance.h"
#include "../structures/tour.h"
#include "../structures/candidate.h"

void addLastNode(InstanceData &data, Tour &tour, int node = 0) {
    tour.cost += data.cost[tour.path.back()][0];
    tour.path.push_back(0);
}

void setNotVisited(vector<bool> &visited, vector<int> &notVisited) {
    for (int i = 0; i < visited.size(); i++) {
        if (!visited[i]) {
            notVisited.push_back(i);
        }
    }
}

FeasibleSol constructive(InstanceData data, int K, double minPrize, double minProb) {
    int n = data.size;
    
    Tour feasibleTour = { {0}, 0.0, 0.0 };
    vector<bool> visited(n, false); 
    vector<int> notVisited;

    visited[0] = true;

    for (int index = 0; index < n; index++)
    {
        vector<Candidate> candidates;  
        int lastNode = feasibleTour.path.back();

        for (int i = 0; i < n; i++) 
        {
            if (!visited[i]) 
            {
                double attraction = data.probability[i];
                Candidate newCandidate = {attraction, i};
                candidates.emplace_back(newCandidate);
            }
        }

        sort(candidates.begin(), candidates.end(), 
            [](const Candidate& a, const Candidate& b) {
                return a.attraction > b.attraction; 
            }
        );

        int nextCustormer = candidates[0].id;

        feasibleTour.path.push_back(nextCustormer);
        feasibleTour.cost += data.cost[lastNode][nextCustormer];
        feasibleTour.prize += data.prize[nextCustormer];

        visited[nextCustormer] = true;
    }   
    

    addLastNode(data, feasibleTour);
    setNotVisited(visited, notVisited);
    
    return { 
        feasibleTour, 
        notVisited 
    };
}
