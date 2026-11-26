#include "BinPackingDomain.hpp"
#include "HyperHeuristic.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

struct TestCase {
    std::string name;
    int numItems;
    int minWeight;
    int maxWeight;
    int binCapacity;
    int iterations;
    unsigned int seed;
};

struct BenchmarkResult {
    int initialBins;
    double initialObj;
    int localBins;
    double localObj;
    int hhBins;
    double hhObj;
    double hhTimeMs;
};

// Lee los escenarios desde un archivo CSV
std::vector<TestCase> loadTestCasesFromCSV(const std::string& filepath) {
    std::vector<TestCase> suite;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de pruebas: " << filepath << "\n";
        return suite;
    }

    std::string line;
    std::getline(file, line); // Saltar la fila de encabezados

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        TestCase tc;

        std::getline(ss, tc.name, ',');
        std::getline(ss, token, ','); tc.numItems = std::stoi(token);
        std::getline(ss, token, ','); tc.minWeight = std::stoi(token);
        std::getline(ss, token, ','); tc.maxWeight = std::stoi(token);
        std::getline(ss, token, ','); tc.binCapacity = std::stoi(token);
        std::getline(ss, token, ','); tc.iterations = std::stoi(token);
        std::getline(ss, token, ','); tc.seed = static_cast<unsigned int>(std::stoul(token));

        suite.push_back(tc);
    }
    return suite;
}

std::vector<int> generateWeights(const TestCase& tc) {
    std::mt19937 gen(tc.seed);
    std::uniform_int_distribution<int> dist(tc.minWeight, tc.maxWeight);
    std::vector<int> weights(tc.numItems);
    for (int& w : weights) {
        w = dist(gen);
    }
    return weights;
}

BenchmarkResult runBenchmark(const TestCase& tc) {
    std::vector<int> weights = generateWeights(tc);
    BenchmarkResult res{};

    // 1. Solo vecinos locales (H0 + H1)
    BinPackingDomain localProb(tc.binCapacity, weights, 42);
    res.initialObj = localProb.getObjectiveValue();
    res.initialBins = localProb.getBinCount();

    double bestLocal = res.initialObj;
    for (int i = 0; i < tc.iterations; ++i) {
        localProb.applyHeuristic(i % 2);
        double cost = localProb.getObjectiveValue();
        if (cost <= bestLocal) {
            bestLocal = cost;
            localProb.saveState();
        } else {
            localProb.restoreState();
        }
    }
    res.localObj = bestLocal;
    res.localBins = localProb.getBinCount();

    // 2. Hiperheurística completa (H0 + H1 + H2)
    BinPackingDomain hhProb(tc.binCapacity, weights, 42);
    HyperHeuristic hh(hhProb);

    auto start = std::chrono::high_resolution_clock::now();
    hh.run(tc.iterations);
    auto end = std::chrono::high_resolution_clock::now();

    res.hhObj = hhProb.getObjectiveValue();
    res.hhBins = hhProb.getBinCount();
    res.hhTimeMs = std::chrono::duration<double, std::milli>(end - start).count();

    return res;
}

int main() {
    // Ajuste automático de ruta si el ejecutable corre desde /build
    std::string inputPath = fs::exists("pruebas/escenarios.csv") 
                            ? "pruebas/escenarios.csv" 
                            : "../pruebas/escenarios.csv";

    std::string outputDir = fs::exists("pruebas") ? "resultados" : "../resultados";
    fs::create_directories(outputDir); // Crea la carpeta resultados/ si no existe

    std::vector<TestCase> suite = loadTestCasesFromCSV(inputPath);
    if (suite.empty()) return 1;

    std::string outputPath = outputDir + "/comparativa.csv";
    std::ofstream outCSV(outputPath);
    if (!outCSV.is_open()) {
        std::cerr << "Error: No se pudo crear el archivo de salida: " << outputPath << "\n";
        return 1;
    }

    // Escribir encabezados del CSV de salida
    outCSV << "Escenario,Items,Capacidad,Iteraciones,Bins_Inicial,Obj_Inicial,"
           << "Bins_Local,Obj_Local,Bins_HiperHeuristica,Obj_HiperHeuristica,"
           << "Ahorro_Bins,Tiempo_ms\n";

    for (const auto& tc : suite) {
        BenchmarkResult r = runBenchmark(tc);
        int binsSaved = r.localBins - r.hhBins;

        outCSV << tc.name << ","
               << tc.numItems << ","
               << tc.binCapacity << ","
               << tc.iterations << ","
               << r.initialBins << ","
               << r.initialObj << ","
               << r.localBins << ","
               << r.localObj << ","
               << r.hhBins << ","
               << r.hhObj << ","
               << binsSaved << ","
               << r.hhTimeMs << "\n";
    }

    outCSV.close();
    std::cout << "Experimento finalizado. Resultados exportados a: " << outputPath << "\n";

    return 0;
}