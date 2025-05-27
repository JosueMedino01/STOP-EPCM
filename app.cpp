#include <iostream>
#include <string.h>

/* #include "greedy-algorithm/GreedyAlgorithm.h" */
#include "utils/Structs.h"
#include "utils/ReadInstance.h"
#include "iterated-local-search/IteratedLocalSearch.h"
#include "evaluate-tour-probability/EvaluateTourProbability.h"

void printData(Tour tour, vector<int> notVisited);

void readArguments(int argc, char *argv[], string &filename, int &seed, 
    int &K, double &C, int &TABU_TENURE, int &MAX_ITER_TABU, int &MAX_NOT_IMPROVIMENT
);

int main(int argc, char *argv[]) {
    string filename = "";
    int seed, K, TABU_TENURE, MAX_ITER_TABU, MAX_NOT_IMPROVIMENT;
    double C;

    readArguments(argc, argv, filename, seed, K, C, TABU_TENURE, MAX_ITER_TABU, MAX_NOT_IMPROVIMENT);

    InstanceData data = readFile(filename); 
    IteratedLocalSearch ils(MAX_NOT_IMPROVIMENT, K, C, seed);
    ils.run(data, K, C);

    /* EvaluateTourProbability evaluate;
    double prob = evaluate.evaluate(
        ils.bestSolution.feasibleTour.path.size() - 1, 
        5, 
        ils.bestSolution.feasibleTour.path, 
        data.probability, 
        data.prize
    ); */

    /* cout << "Probabilidade: " << prob << endl; */
    cout << "Custo: " << ils.bestSolution.feasibleTour.cost << endl;
    cout << "Premio: " << ils.bestSolution.feasibleTour.prize << endl;

    
   
    /* vector<int> path = ils.bestSolution.feasibleTour.path;
    vector<double> probs = data.probability;
    vector<int> prizes = data.prize;
    int size = path.size();
    int Pmin = 8000;

    cout << "path: ";
    for (int i= 0; i < size; i++)
    {
        cout << path[i] << ",";
    }
    cout << endl;
    cout << "probs: ";
    for (int i= 0; i < size; i++)
    {
        cout << probs[i] << ",";
    }
    cout << endl;
    cout << "prizes: ";
    for (int i= 0; i < size; i++)
    {
        cout << prizes[i] << ",";
    }
    cout << endl;
    
    EvaluateTourProbability evaluate;
    double res = evaluate.evaluate(size, Pmin, path, probs, prizes);

    cout << "Valor : " << res << endl;
 */
    
    return 0;
}




void readArguments(int argc, char *argv[], string &filename, int &seed, 
    int &K, double &C, int &TABU_TENURE, int &MAX_ITER_TABU, int &MAX_NOT_IMPROVIMENT) 
{
    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-f"))
        {
            filename = argv[i + 1];
        }
        else if (!strcmp(argv[i], "-C"))
        {
            C = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-K"))
        {
            K = atoi(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-T"))
        {
            TABU_TENURE = atoi(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-maxIterTabu"))
        {
            MAX_ITER_TABU = atoi(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-maxNotImproviment"))
        {
            MAX_NOT_IMPROVIMENT = atoi(argv[i + 1]);
        } else if (!strcmp(argv[i], "-seed")) {
            seed = atoi(argv[i + 1]);
        }
        
    }

};

void printData(Tour tour, vector<int> notVisited) {
    ofstream outFile("solution_log.txt", ios::app); // Abre o arquivo em modo append

    if (!outFile) {
        cerr << "Erro ao abrir o arquivo para escrita!" << endl;
        return;
    }

    outFile << "Tour: ";
    for (int i = 0; i < tour.path.size(); i++) {
        outFile << tour.path[i] << " ";
    }
    
    outFile << "\nNot visited: ";
    for (int i = 0; i < notVisited.size(); i++) {
        outFile << notVisited[i] << " ";
    }

    outFile << "\nCost: " << tour.cost;
    outFile << "\nPrize: " << tour.prize;
    

    outFile.close(); // Fecha o arquivo corretamente
}