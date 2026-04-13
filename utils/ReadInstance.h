#ifndef READ_INSTANCE_H 
#define READ_INSTANCE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>
#include "Structs.h"

using namespace std;

double getCost(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

InstanceData readFile(string path) {
    InstanceData data;
    ifstream file(path);
    string line;

    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo." << endl;
        return data;
    }

    while (getline(file, line)) {
        istringstream iss(line);
        string key;
        char colon;

        iss >> key >> colon;

        if (key == "DIMENSION") {
            int value;
            iss >> value;
            data.size = value;
            data.cost.resize(data.size, vector<double>(data.size, 0));
            data.probability.resize(data.size, 0.0);
            data.prize.resize(data.size, 0);
        }
        else if (key == "TMAX") {
            int value;
            iss >> value;
            data.deadline = value;
        }
        else if (key == "MIN_PRIZE:") {
            double value;
            iss >> value;
            data.MIN_PRIZE = value;
        }
        else if (key == "MIN_PROB:") {
            double value;
            iss >> value;
            data.MIN_PROB = value;
        }
        else if (key == "NODE_COORD_SECTION") {
            vector<pair<int, int>> coords(data.size);

            for (int i = 0; i < data.size; i++) {
                int id, x, y;
                file >> id >> x >> y;
                coords[id - 1] = {x, y};
            }

            for (int i = 0; i < data.size; i++) {
                for (int j = 0; j < data.size; j++) {
                    data.cost[i][j] = getCost(coords[i].first, coords[i].second,
                                               coords[j].first, coords[j].second);
                }
            }
        }
        else if (key == "EDGE_WEIGHT_SECTION") {
            for (int i = 0; i < data.size; i++) {
                for (int j = i + 1; j < data.size; j++) {
                    int cost;
                    file >> cost;
                    data.cost[i][j] = cost;
                    data.cost[j][i] = cost;
                }
            }
        }
        else if (key == "NODE_PRIZE_PROBABILITY_SECTION") {
            for (int i = 0; i < data.size; i++) {
                int id, prize;
                double prob;

                file >> id >> prize >> prob;
                // CORRECAO: Usar id-1 para converter de 1-indexed para 0-indexed
                data.probability[id - 1] = prob;
                data.prize[id - 1] = prize;
            }
        }
    }

    file.close();
    return data;
}
#endif
