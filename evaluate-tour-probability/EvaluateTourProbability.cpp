#include "EvaluateTourProbability.h"
#include "../utils/Structs.h"
#include <iostream>
#include <map>

int contador = 0;
map<pair<int,int>, double> hashMap;

double EvaluateTourProbability::evaluate(int i, int Pmin, vector<int> path, 
    vector<double> probabilities, vector<int> prizes) 
{
    for (int k= 0; k < i; k++)
    {
        cout << path[k] << ",";
    }

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

    double prob_i = (probabilities[i - 1] > 0) ? probabilities[i - 1] : 0; 

    double prob = evaluate(i-1, Pmin - prizes[path[i - 1]], path, probabilities, prizes) * prob_i +  
            evaluate(i-1, Pmin, path, probabilities, prizes) * (1 - prob_i);

    hashMap[{i, Pmin}] = prob;

    return prob;
}
