import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

# ============================================================
# KONFIGURACJA
# ============================================================

# Pliki CSV dla dwóch implementacji grafu
CSV_LIST = "results_adj_list.csv"
CSV_MATRIX = "results_adj_matrix.csv"

# Nazwy implementacji wyświetlane na wykresach
IMPLEMENTATION_NAMES = {
    "list": "Adjacency List",
    "matrix": "Adjacency Matrix"
}

# Folder wyjściowy
OUTPUT_DIR = Path("plots")
OUTPUT_DIR.mkdir(exist_ok=True)

# ============================================================
# WCZYTYWANIE DANYCH
# ============================================================

df_list = pd.read_csv(CSV_LIST)
df_matrix = pd.read_csv(CSV_MATRIX)

# Dodanie kolumny identyfikującej implementację
df_list["Implementation"] = IMPLEMENTATION_NAMES["list"]
df_matrix["Implementation"] = IMPLEMENTATION_NAMES["matrix"]

# Połączenie danych
df = pd.concat([df_list, df_matrix], ignore_index=True)

# ============================================================
# FILTROWANIE — tylko Bellman-Ford
# ============================================================

# Dostosuj nazwę algorytmu jeśli w CSV masz inną
df = df[df["Algorithm"].str.contains("Bellman", case=False)]

# ============================================================
# UŚREDNIANIE WYNIKÓW
# ============================================================

grouped = (
    df.groupby(
        [
            "Implementation",
            "RequestedDensity",
            "Vertices"
        ]
    )["TimeMs"]
    .mean()
    .reset_index()
)

# ============================================================
# 1. DWA WYKRESY:
#    jeden wykres = jedna implementacja
#    4 linie = 4 gęstości
# ============================================================

implementations = grouped["Implementation"].unique()

for impl in implementations:

    plt.figure(figsize=(10, 6))

    impl_df = grouped[grouped["Implementation"] == impl]

    densities = sorted(impl_df["RequestedDensity"].unique())

    for density in densities:

        density_df = impl_df[
            impl_df["RequestedDensity"] == density
        ].sort_values("Vertices")

        plt.plot(
            density_df["Vertices"],
            density_df["TimeMs"],
            marker='o',
            label=f"Density = {density}"
        )

    plt.title(f"Bellman-Ford — {impl}")
    plt.xlabel("Liczba wierzchołków")
    plt.ylabel("Czas wykonania [ms]")
    plt.grid(True)
    plt.legend()

    plt.tight_layout()

    filename = (
        OUTPUT_DIR /
        f"bellman_ford_{impl.replace(' ', '_').lower()}.png"
    )

    plt.savefig(filename)
    plt.close()

    print(f"Zapisano: {filename}")

# ============================================================
# 2. CZTERY WYKRESY:
#    jeden wykres = jedna gęstość
#    2 linie = 2 implementacje
# ============================================================

densities = sorted(grouped["RequestedDensity"].unique())

for density in densities:

    plt.figure(figsize=(10, 6))

    density_df = grouped[
        grouped["RequestedDensity"] == density
    ]

    for impl in implementations:

        impl_df = density_df[
            density_df["Implementation"] == impl
        ].sort_values("Vertices")

        plt.plot(
            impl_df["Vertices"],
            impl_df["TimeMs"],
            marker='o',
            label=impl
        )

    plt.title(f"Bellman-Ford — Density = {density}")
    plt.xlabel("Liczba wierzchołków")
    plt.ylabel("Czas wykonania [ms]")
    plt.grid(True)
    plt.legend()

    plt.tight_layout()

    filename = (
        OUTPUT_DIR /
        f"bellman_ford_density_{density}.png"
    )

    plt.savefig(filename)
    plt.close()

    print(f"Zapisano: {filename}")

# ============================================================
# GOTOWE
# ============================================================

print("\nWszystkie wykresy zostały wygenerowane.")