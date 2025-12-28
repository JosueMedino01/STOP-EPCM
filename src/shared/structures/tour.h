#ifndef TOUR_H
#define TOUR_H

#include <string>
#include <vector>

using namespace std;

struct Tour
{
    vector<int> path;
    double cost = 0;
    int prize = 0;
};

struct FeasibleSol {
    Tour feasibleTour;
    vector<int> notVisited;
};
#endif
