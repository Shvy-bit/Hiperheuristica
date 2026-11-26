#pragma once
#include "ProblemDomain.hpp"
#include <vector>

class HyperHeuristic {
private:
    ProblemDomain& domain;
    std::vector<double> scores;          // Puntuación dinámica de cada heurística
    std::vector<int> selectionCounts;    // Estadística de uso
    std::vector<int> improvementCounts;  // Estadística de éxito

    int selectNextHeuristic();

public:
    explicit HyperHeuristic(ProblemDomain& problemDomain);

    // Ejecuta el ciclo de optimización durante N iteraciones
    void run(int maxIterations);

    // Muestra qué heurísticas priorizó el sistema
    void printStatistics() const;
};