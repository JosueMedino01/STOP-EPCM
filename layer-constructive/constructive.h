#ifndef CONSTRUCTIVE_H
#define CONSTRUCTIVE_H

#pragma once

#include "../utils/Structs.h"

class Constructive
{   
    private:
        double probabilityAttraction(InstanceData &data, int i, int j, double C);

    public:
        /**
         * Returns a feasible tour using the K-Neighbor Probability Insertion heuristic.
         */
        Customers KNeighborProbabilityInsertion(InstanceData &data, int K, double C, double minPrize, double minProb);
        //Customers kNeighborRandomInsertion(InstanceData data, int K, double C, double minPrize, double minProb);

};

#endif