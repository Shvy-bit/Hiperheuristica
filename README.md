# Optimización de Empaquetamiento (Bin Packing 1D) mediante Hiperheurísticas en C++

Este proyecto implementa una arquitectura modular de **Hiperheurística de Selección** en C++17 aplicada al problema de empaquetamiento en contenedores (*1D Bin Packing Problem*). 

El objetivo es demostrar cómo un selector de alto nivel puede escapar de óptimos locales profundos combinando **vecinos locales simples** con un **vecino estructural no trivial** (*Ruin & Recreate + First-Fit Decreasing*), respetando estrictamente la **barrera de dominio**.

---

## Arquitectura del Proyecto

```text
proyecto_hiperheuristica/
├── CMakeLists.txt               # Configuración de compilación con CMake
├── .gitignore                   # Exclusión de binarios, cachés y entornos virtuales
├── include/                     # Archivos de cabecera (.hpp)
│   ├── ProblemDomain.hpp        # Interfaz pura (Barrera de dominio)
│   ├── BinPackingDomain.hpp     # Definición del problema y heurísticas de bajo nivel
│   └── HyperHeuristic.hpp       # Definición del motor selector adaptativo
├── src/                         # Implementaciones (.cpp)
│   ├── BinPackingDomain.cpp     # Lógica de empaquetamiento y operadores de vecindad
│   ├── HyperHeuristic.cpp       # Lógica de puntuación, aceptación y decaimiento
│   └── main.cpp                 # Ejecución del benchmark y exportación a CSV
├── pruebas/
│   └── escenarios.csv           # Instancias de prueba configurables
├── resultados/
│   └── comparativa.csv          # Métricas generadas automáticamente tras la ejecución
├── tools/
│   └── graficar_resultados.py   # Script en Python para generar gráficos comparativos
└── figuras/                     # Gráficos exportados en alta resolución (PNG)
```

### Heurísticas de Bajo Nivel Implementadas
1. **`H0` (Vecino Local Directo):** Mueve un objeto aleatorio hacia otro contenedor con capacidad disponible.
2. **`H1` (Vecino Local Directo):** Intercambia dos objetos entre dos contenedores distintos respetando la capacidad máxima.
3. **`H2` (Vecino Estructural No Trivial):** *Ruin & Recreate parcial*. Destruye los contenedores con menor porcentaje de ocupación, ordena los objetos liberados de mayor a menor peso y los reinserta mediante *First-Fit Decreasing (FFD)*.

---

## Compilación y Ejecución (C++)

Puedes compilar el proyecto utilizando **CMake** o directamente con el compilador **`g++` / `clang++`** si no tienes CMake instalado.

### Opción 1: Compilación usando CMake (Recomendado)

**En Linux / macOS:**
```bash
mkdir -p build && cd build
cmake ..
cmake --build .
./hh_demo
```

**En Windows (PowerShell):**
```powershell
mkdir build -Force
cd build
cmake ..
cmake --build .
.\Debug\hh_demo.exe
```

### Opción 2: Compilación directa sin CMake (usando `g++`)

Desde la raíz del proyecto, compila todos los archivos fuente indicando la carpeta `-Iinclude`:

**En Linux / macOS:**
```bash
g++ -std=c++17 -O2 -Iinclude src/main.cpp src/BinPackingDomain.cpp src/HyperHeuristic.cpp -o hh_demo
./hh_demo
```

**En Windows (MinGW / PowerShell / CMD):**
```powershell
g++ -std=c++17 -O2 -Iinclude src/main.cpp src/BinPackingDomain.cpp src/HyperHeuristic.cpp -o hh_demo.exe
.\hh_demo.exe
```

---

## Generación de Gráficos

Una vez generado `resultados/comparativa.csv`, crea un entorno virtual y ejecuta el script de visualización:

**En Windows (PowerShell):**
```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install pandas matplotlib
python tools/graficar_resultados.py
```

**En Linux / macOS:**
```bash
python3 -m venv .venv
source .venv/bin/activate
pip install pandas matplotlib
python tools/graficar_resultados.py
```