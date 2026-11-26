#pragma once
#include <string>

class ProblemDomain {
public:
    virtual ~ProblemDomain() = default;

    // Inicializa o reinicia una solución base
    virtual void initializeSolution() = 0;

    // Devuelve el costo actual (a minimizar)
    virtual double getObjectiveValue() const = 0;

    // Devuelve cuántas heurísticas de bajo nivel existen
    virtual int getNumberOfHeuristics() const = 0;

    // Aplica la heurística indicada por su índice (0 a N-1)
    virtual void applyHeuristic(int heuristicIndex) = 0;

    // Guarda y restaura el estado de la solución (para aceptar/rechazar movimientos)
    virtual void saveState() = 0;
    virtual void restoreState() = 0;

    // Opcional: solo para mostrar métricas al usuario al final
    virtual std::string getHeuristicName(int heuristicIndex) const = 0;
    virtual void printSummary() const = 0;
};