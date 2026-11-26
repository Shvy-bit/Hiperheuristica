import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Rutas dinámicas respecto a la raíz del proyecto
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CSV_PATH = os.path.join(BASE_DIR, "resultados", "comparativa.csv")
FIGURAS_DIR = os.path.join(BASE_DIR, "figuras")

def main():
    if not os.path.exists(CSV_PATH):
        print(f"Error: No se encontró el archivo {CSV_PATH}")
        print("Asegúrate de ejecutar primero el programa en C++.")
        return

    # Crear la carpeta figuras/ automáticamente si no existe
    os.makedirs(FIGURAS_DIR, exist_ok=True)

    # Leer los datos
    df = pd.read_csv(CSV_PATH)

    # Configurar estilo limpio
    plt.style.use("seaborn-v0_8-whitegrid")

    # =========================================================================
    # GRÁFICO 1: Comparativa de Contenedores Utilizados (Menor es mejor)
    # =========================================================================
    fig, ax = plt.subplots(figsize=(12, 6))

    x = np.arange(len(df["Escenario"]))
    width = 0.26

    rects1 = ax.bar(x - width, df["Bins_Inicial"], width, label="Inicial (Aleatorio)", color="#95a5a6")
    rects2 = ax.bar(x, df["Bins_Local"], width, label="Solo Vecinos Locales (H0 + H1)", color="#e67e22")
    rects3 = ax.bar(x + width, df["Bins_HiperHeuristica"], width, label="Hiperheurística (H0 + H1 + H2)", color="#2980b9")

    ax.set_ylabel("Número de Contenedores (Bins)", fontsize=11, fontweight="bold")
    ax.set_title("Comparativa de Empaquetamiento: Vecinos Locales vs. Hiperheurística", fontsize=13, fontweight="bold", pad=15)
    ax.set_xticks(x)
    ax.set_xticklabels(df["Escenario"], rotation=25, ha="right", fontsize=10)
    ax.legend(frameon=True, facecolor="white", fontsize=10)

    # Añadir etiquetas numéricas encima de cada barra
    for rects in [rects1, rects2, rects3]:
        for rect in rects:
            height = rect.get_height()
            ax.annotate(f"{int(height)}",
                        xy=(rect.get_x() + rect.get_width() / 2, height),
                        xytext=(0, 3), textcoords="offset points",
                        ha="center", va="bottom", fontsize=8.5, fontweight="bold")

    plt.tight_layout()
    out_path_1 = os.path.join(FIGURAS_DIR, "comparativa_contenedores.png")
    plt.savefig(out_path_1, dpi=300)
    plt.close()

    # =========================================================================
    # GRÁFICO 2: Ahorro de Contenedores y Tiempo de Ejecución
    # =========================================================================
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5.5))

    # Panel izquierdo: Contenedores ahorrados frente a búsqueda local
    bars_saved = ax1.bar(df["Escenario"], df["Ahorro_Bins"], color="#27ae60", width=0.5)
    ax1.set_title("Contenedores Ahorrados por la Hiperheurística\n(vs. Búsqueda Local Tradicional)", fontsize=11, fontweight="bold")
    ax1.set_ylabel("Contenedores Extra Ahorrados", fontsize=10, fontweight="bold")
    ax1.tick_params(axis="x", rotation=30)
    for tick in ax1.get_xticklabels():
        tick.set_horizontalalignment("right")

    for bar in bars_saved:
        h = bar.get_height()
        ax1.annotate(f"+{int(h)}",
                     xy=(bar.get_x() + bar.get_width() / 2, h),
                     xytext=(0, 3), textcoords="offset points",
                     ha="center", va="bottom", fontsize=9, fontweight="bold", color="#1e8449")

    # Panel derecho: Tiempo de ejecución (ms)
    ax2.plot(df["Escenario"], df["Tiempo_Local_ms"], marker="s", linestyle="--",
             linewidth=2.2, markersize=7, color="#e67e22", label="Solo Vecinos Locales (H0 + H1)")
    ax2.plot(df["Escenario"], df["Tiempo_HH_ms"], marker="o", linestyle="-",
             linewidth=2.5, markersize=8, color="#8e44ad", label="Hiperheurística (H0 + H1 + H2)")

    ax2.set_title("Comparativa de Tiempo de Ejecución", fontsize=11, fontweight="bold")
    ax2.set_ylabel("Tiempo (milisegundos)", fontsize=10, fontweight="bold")
    ax2.legend(frameon=True, facecolor="white", fontsize=9.5, loc="upper left")
    ax2.tick_params(axis="x", rotation=30)
    for tick in ax2.get_xticklabels():
        tick.set_horizontalalignment("right")

    for i in range(len(df)):
        ax2.annotate(f"{df['Tiempo_HH_ms'].iloc[i]:.1f} ms",
                     (df["Escenario"].iloc[i], df["Tiempo_HH_ms"].iloc[i]),
                     textcoords="offset points", xytext=(0, 8),
                     ha="center", fontsize=8.5, color="#6c3483", fontweight="bold")
        ax2.annotate(f"{df['Tiempo_Local_ms'].iloc[i]:.1f} ms",
                     (df["Escenario"].iloc[i], df["Tiempo_Local_ms"].iloc[i]),
                     textcoords="offset points", xytext=(0, -14),
                     ha="center", fontsize=8, color="#d35400")
                     
    plt.tight_layout()
    out_path_2 = os.path.join(FIGURAS_DIR, "ahorro_y_tiempo.png")
    plt.savefig(out_path_2, dpi=300)
    plt.close()

    print(f"Gráficos generados exitosamente en:\n - {out_path_1}\n - {out_path_2}")

if __name__ == "__main__":
    main()