#include "HyperHeuristic.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>

HyperHeuristic::HyperHeuristic(ProblemDomain& problemDomain)
    : domain(problemDomain) {
    int n = domain.getNumberOfHeuristics();
    scores.assign(n, 10.0); // Todas empiezan con la misma confianza inicial
    selectionCounts.assign(n, 0);
    improvementCounts.assign(n, 0);
}

int HyperHeuristic::selectNextHeuristic() {
    // Selección determinista por mayor puntuación (con exploración implícita por decaimiento)
    int bestIdx = 0;
    for (size_t i = 1; i < scores.size(); ++i) {
        if (scores[i] > scores[bestIdx]) {
            bestIdx = static_cast<int>(i);
        }
    }
    return bestIdx;
}

void HyperHeuristic::run(int maxIterations) {
    double currentBestCost = domain.getObjectiveValue();
    domain.saveState();

    for (int iter = 0; iter < maxIterations; ++iter) {
        int h = selectNextHeuristic();
        selectionCounts[h]++;

        domain.applyHeuristic(h);
        double newCost = domain.getObjectiveValue();

        if (newCost < currentBestCost - 1e-9) {
            // Mejora estricta: aceptamos el cambio y premiamos fuertemente la heurística
            double delta = currentBestCost - newCost;
            currentBestCost = newCost;
            domain.saveState();

            scores[h] += 5.0 + (delta * 10.0);
            improvementCounts[h]++;
        } else if (std::abs(newCost - currentBestCost) <= 1e-9) {
            // Movimiento neutro (meseta): aceptamos para explorar, leve decaimiento
            domain.saveState();
            scores[h] = std::max(1.0, scores[h] * 0.95);
        } else {
            // Empeora la solución: revertimos el estado y penalizamos la heurística
            domain.restoreState();
            scores[h] = std::max(1.0, scores[h] * 0.80);
        }
    }
}

void HyperHeuristic::printStatistics() const {
    std::cout << "\n--- Estadisticas de Seleccion Hiperheuristica ---\n";
    for (size_t i = 0; i < scores.size(); ++i) {
        std::cout << std::left << std::setw(42) << domain.getHeuristicName(static_cast<int>(i))
                  << " | Veces elegida: " << std::setw(4) << selectionCounts[i]
                  << " | Mejoras logradas: " << std::setw(4) << improvementCounts[i]
                  << " | Score final: " << std::fixed << std::setprecision(2) << scores[i] << "\n";
    }
}