#include "constructive.h"
#include "../utils/ReadInstance.h"
#include "../utils/Structs.h"

int main () {
    Constructive constructive;

    InstanceData data = readFile("layer-instances/Artificial_POP_Instance_01.txt");
    
    Customers result = constructive.KNeighborProbabilityInsertion(data, 5, 0.5, 10, 0.1);
    return 0;
}