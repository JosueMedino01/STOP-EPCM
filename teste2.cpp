#include <iostream>
#include <vector>
#include <map>
using namespace std;

map<pair<int, int>, double> hashMap;

double evaluate(int i, int Pmin, const vector<int>& path, const vector<double>& probs, const vector<int>& prizes) {
    if (Pmin <= 0) return 1.0;
    if (i < 0) return 0.0;
    if (hashMap.count({i, Pmin})) return hashMap[{i, Pmin}];

    int cliente = path[i];
    double prob_cliente = (probs[cliente] < 0) ? 0.0 : probs[cliente]; // Probabilidade negativa -> 0
    double prob_visitado = 0.0;
    double prob_nao_visitado = 0.0;

    if (Pmin - prizes[cliente] <= 0) {
        prob_visitado = prob_cliente * 1.0;
    } else {
        prob_visitado = prob_cliente * evaluate(i-1, Pmin - prizes[cliente], path, probs, prizes);
    }

    prob_nao_visitado = (1 - prob_cliente) * evaluate(i-1, Pmin, path, probs, prizes);
    double total = prob_visitado + prob_nao_visitado;
    hashMap[{i, Pmin}] = total;
    return total;
}

int main() {
    vector<int> path = {0,48,31,44,18,40,7,8,9,42,50,10,51,13,12,46,25,26,27,11,24,3,5,14,4,23,47,37,36,39,38,35,34,33,43,15,49,19,29,1,6,41,20,30,0};
    vector<double> probs = {1,1,1,1,-1,-1,-1,-1,-1,-1,-1,-0.4,-0.4,-0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4};
    vector<int> prizes = {0,667,282,396,292,327,641,427,601,562,1041,656,975,1121,300,261,430,162,305,211,287,47,182,275,411,729,799,708,407,361,147,91,828,136,122,125,208,241,167,209,396,566,464,155,241};

    int Pmin = 5000; // Defina o prêmio mínimo desejado
    double resultado = evaluate(path.size() - 1, Pmin, path, probs, prizes);
    cout << "Probabilidade de atingir Pmin: " << resultado << endl;

    return 0;
}