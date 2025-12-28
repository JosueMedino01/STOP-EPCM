#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include <vector>

using namespace std;

struct InstanceData {
    int size;
    int deadline;
    vector<double> probability;
    vector<double> prize;
    vector<vector<double>> cost;
};

#endif
