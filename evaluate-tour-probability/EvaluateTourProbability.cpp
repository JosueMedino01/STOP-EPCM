#include "EvaluateTourProbability.h"
#include "../utils/Structs.h"
#include <iostream>
#include <map>

map<pair<int,int>, double> hashMap;

double EvaluateTourProbability::evaluate(int i, double Pmin, const vector<int> &path, 
    const vector<double> &probabilities, const vector<double> &prizes) 
{
    if (Pmin <= 0) 
    {
        return 1.0;
    } 
    else if (i == 0) 
    {
        return 0.0;
    }
    else if (hashMap.find({i, Pmin}) != hashMap.end())
    {
        return hashMap.find({i, Pmin})->second;
    }

    int node = path[i - 1];
    double prob_i = (probabilities[node] > 0) ? probabilities[node] : 0; 

    double prob = evaluate(i-1, Pmin - prizes[node], path, probabilities, prizes) * prob_i +  
            evaluate(i-1, Pmin, path, probabilities, prizes) * (1 - prob_i);

    hashMap[{i, Pmin}] = prob;

    return prob;
}
