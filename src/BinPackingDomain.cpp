#include "BinPackingDomain.hpp"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cmath>

BinPackingDomain::BinPackingDomain(int capacity, const std::vector<int>& weights, unsigned int seed)
    : binCapacity(capacity), itemWeights(weights), rng(seed) {
    initializeSolution();
}

void BinPackingDomain::initializeSolution() {
    currentBins.clear();
    // Inicialización deliberadamente subóptima (Next-Fit en orden aleatorio)
    std::vector<int> indices(itemWeights.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), rng);

    for (int idx : indices) {
        if (currentBins.empty() || getBinLoad(currentBins.back()) + itemWeights[idx] > binCapacity) {
            currentBins.push_back({idx});
        } else {
            currentBins.back().push_back(idx);
        }
    }
    saveState();
}

int BinPackingDomain::getBinLoad(const std::vector<int>& bin) const {
    int total = 0;
    for (int idx : bin) {
        total += itemWeights[idx];
    }
    return total;
}

void BinPackingDomain::cleanEmptyBins(std::vector<std::vector<int>>& bins) {
    bins.erase(
        std::remove_if(bins.begin(), bins.end(), [](const std::vector<int>& b) { return b.empty(); }),
        bins.end()
    );
}

// Función objetivo (a minimizar):
// Penaliza cantidad de contenedores y premia contenedores lo más llenos posible
double BinPackingDomain::getObjectiveValue() const {
    double utilizationScore = 0.0;
    for (const auto& bin : currentBins) {
        double ratio = static_cast<double>(getBinLoad(bin)) / binCapacity;
        utilizationScore += (ratio * ratio);
    }
    // Minimizar: número de bins menos la calidad de empaquetamiento
    return static_cast<double>(currentBins.size()) - (utilizationScore / currentBins.size());
}

int BinPackingDomain::getNumberOfHeuristics() const {
    return 3;
}

void BinPackingDomain::applyHeuristic(int heuristicIndex) {
    switch (heuristicIndex) {
        case 0: heuristicMoveSingleItem(); break;
        case 1: heuristicSwapTwoItems(); break;
        case 2: heuristicRuinAndRecreate(); break;
        default: break;
    }
}

// H0: Mover un objeto aleatorio a otro contenedor donde quepa (vecindad directa)
void BinPackingDomain::heuristicMoveSingleItem() {
    if (currentBins.size() < 2) return;

    std::uniform_int_distribution<size_t> binDist(0, currentBins.size() - 1);
    size_t srcBin = binDist(rng);
    if (currentBins[srcBin].empty()) return;

    std::uniform_int_distribution<size_t> itemDist(0, currentBins[srcBin].size() - 1);
    size_t itemPos = itemDist(rng);
    int itemIdx = currentBins[srcBin][itemPos];

    for (size_t dstBin = 0; dstBin < currentBins.size(); ++dstBin) {
        if (dstBin == srcBin) continue;
        if (getBinLoad(currentBins[dstBin]) + itemWeights[itemIdx] <= binCapacity) {
            currentBins[dstBin].push_back(itemIdx);
            currentBins[srcBin].erase(currentBins[srcBin].begin() + itemPos);
            cleanEmptyBins(currentBins);
            return;
        }
    }
}

// H1: Intercambiar dos objetos de dos contenedores distintos (vecindad directa)
void BinPackingDomain::heuristicSwapTwoItems() {
    if (currentBins.size() < 2) return;

    std::uniform_int_distribution<size_t> binDist(0, currentBins.size() - 1);
    size_t b1 = binDist(rng);
    size_t b2 = binDist(rng);
    if (b1 == b2 || currentBins[b1].empty() || currentBins[b2].empty()) return;

    std::uniform_int_distribution<size_t> dist1(0, currentBins[b1].size() - 1);
    std::uniform_int_distribution<size_t> dist2(0, currentBins[b2].size() - 1);

    size_t pos1 = dist1(rng);
    size_t pos2 = dist2(rng);

    int item1 = currentBins[b1][pos1];
    int item2 = currentBins[b2][pos2];

    int newLoad1 = getBinLoad(currentBins[b1]) - itemWeights[item1] + itemWeights[item2];
    int newLoad2 = getBinLoad(currentBins[b2]) - itemWeights[item2] + itemWeights[item1];

    if (newLoad1 <= binCapacity && newLoad2 <= binCapacity) {
        currentBins[b1][pos1] = item2;
        currentBins[b2][pos2] = item1;
    }
}

// H2: Vecino NO trivial (Ruin & Recreate parcial)
// Rompe la estructura de los contenedores menos llenos y reacomoda sus piezas
// de mayor a menor peso (First-Fit Decreasing) sobre toda la solución.
void BinPackingDomain::heuristicRuinAndRecreate() {
    if (currentBins.size() < 3) return;

    // Ordenar contenedores de menor a mayor carga
    std::sort(currentBins.begin(), currentBins.end(), [this](const auto& a, const auto& b) {
        return getBinLoad(a) < getBinLoad(b);
    });

    // Destruir los 2 contenedores con peor aprovechamiento (o hasta el 25% de los bins)
    size_t binsToRuin = std::max<size_t>(2, currentBins.size() / 4);
    binsToRuin = std::min(binsToRuin, currentBins.size() - 1);

    std::vector<int> looseItems;
    for (size_t i = 0; i < binsToRuin; ++i) {
        looseItems.insert(looseItems.end(), currentBins[i].begin(), currentBins[i].end());
    }
    currentBins.erase(currentBins.begin(), currentBins.begin() + binsToRuin);

    // Ordenar piezas liberadas de mayor a menor peso
    std::sort(looseItems.begin(), looseItems.end(), [this](int a, int b) {
        return itemWeights[a] > itemWeights[b];
    });

    // Reinsertar usando First-Fit en los contenedores supervivientes
    for (int itemIdx : looseItems) {
        bool placed = false;
        for (auto& bin : currentBins) {
            if (getBinLoad(bin) + itemWeights[itemIdx] <= binCapacity) {
                bin.push_back(itemIdx);
                placed = true;
                break;
            }
        }
        if (!placed) {
            currentBins.push_back({itemIdx});
        }
    }
}

void BinPackingDomain::saveState() {
    savedBins = currentBins;
}

void BinPackingDomain::restoreState() {
    currentBins = savedBins;
}

std::string BinPackingDomain::getHeuristicName(int heuristicIndex) const {
    switch (heuristicIndex) {
        case 0: return "H0: Mover 1 objeto (Local)";
        case 1: return "H1: Intercambiar 2 objetos (Local)";
        case 2: return "H2: Ruin & Recreate + FFD (Estructural)";
        default: return "Desconocida";
    }
}

void BinPackingDomain::printSummary() const {
    std::cout << "Contenedores utilizados: " << currentBins.size()
              << " | Valor objetivo: " << getObjectiveValue() << "\n";
}