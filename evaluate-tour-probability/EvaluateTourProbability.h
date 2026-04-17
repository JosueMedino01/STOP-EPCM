#ifndef EVALUATETOURPROBABILITY_H
#define EVALUATETOURPROBABILITY_H
#include <map>
#pragma once

#include "../utils/Structs.h"

class EvaluateTourProbability
{
public:
    double evaluate(int i, int Pmin, const vector<int> &path, const vector<double> &probabilities, const vector<double> &prizes);
    EvaluateTourProbability();
private:
    map<pair<int,int>, double> hashMap;
};

#endif

