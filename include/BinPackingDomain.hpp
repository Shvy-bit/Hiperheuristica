#pragma once
#include "ProblemDomain.hpp"
#include <vector>
#include <random>

class BinPackingDomain : public ProblemDomain {
private:
    int binCapacity;
    std::vector<int> itemWeights;

    // Representación de la solución: bins[i] contiene los índices de los objetos en el contenedor i
    std::vector<std::vector<int>> currentBins;
    std::vector<std::vector<int>> savedBins;

    mutable std::mt19937 rng;

    // Heurísticas de bajo nivel (vecinos)
    void heuristicMoveSingleItem();      // H0: Vecino obvio (mover 1 objeto a otro contenedor)
    void heuristicSwapTwoItems();        // H1: Vecino obvio (intercambiar 2 objetos entre contenedores)
    void heuristicRuinAndRecreate();     // H2: Vecino NO trivial (destruir los contenedores más vacíos y reinsertar con First-Fit Decreasing)

    int getBinLoad(const std::vector<int>& bin) const;
    void cleanEmptyBins(std::vector<std::vector<int>>& bins);

public:
    BinPackingDomain(int capacity, const std::vector<int>& weights, unsigned int seed = 42);

    void initializeSolution() override;
    double getObjectiveValue() const override;
    int getNumberOfHeuristics() const override;
    void applyHeuristic(int heuristicIndex) override;
    void saveState() override;
    void restoreState() override;

    int getBinCount() const { return static_cast<int>(currentBins.size()); }
    std::string getHeuristicName(int heuristicIndex) const override;
    void printSummary() const override;
};