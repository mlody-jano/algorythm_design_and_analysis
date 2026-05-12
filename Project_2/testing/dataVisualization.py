import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

# ============================================================
# PLIKI CSV
# ============================================================

CSV_LIST = "results_list_76.csv"
CSV_MATRIX = "results_mtx_76.csv"

OUTPUT_DIR = Path("plots")
OUTPUT_DIR.mkdir(exist_ok=True)

# ============================================================
# WCZYTYWANIE
# ============================================================

df_list = pd.read_csv(CSV_LIST)
df_matrix = pd.read_csv(CSV_MATRIX)

print("Kolumny LIST:")
print(df_list.columns)

print("\nKolumny MATRIX:")
print(df_matrix.columns)

# ============================================================
# DODANIE IMPLEMENTACJI
# ============================================================

df_list["Implementation"] = "Adjacency List"
df_matrix["Implementation"] = "Adjacency Matrix"

df = pd.concat([df_list, df_matrix], ignore_index=True)

# ============================================================
# JEŚLI NIE MA KOLUMNY RequestedDensity
# ============================================================

if "RequestedDensity" not in df.columns:

    if "Density" in df.columns:
        df["RequestedDensity"] = df["Density"]
    else:
        raise Exception(
            "Brakuje kolumny RequestedDensity lub Density"
        )

# ============================================================
# JEŚLI NIE MA KOLUMNY Vertices
# ============================================================

if "Vertices" not in df.columns:
    raise Exception("Brakuje kolumny Vertices")

# ============================================================
# JEŚLI NIE MA KOLUMNY TimeMs
# ============================================================

if "TimeMs" not in df.columns:

    possible_names = [
        "Time",
        "ExecutionTime",
        "ExecutionTimeMs"
    ]

    found = False

    for name in possible_names:
        if name in df.columns:
            df["TimeMs"] = df[name]
            found = True
            break

    if not found:
        raise Exception("Brakuje kolumny z czasem wykonania")

# ============================================================
# ŚREDNIE WYNIKI
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
# WYKRESY IMPLEMENTACJI
# ============================================================

implementations = grouped["Implementation"].unique()

for impl in implementations:

    plt.figure(figsize=(10, 6))

    impl_df = grouped[grouped["Implementation"] == impl]

    densities = sorted(
        impl_df["RequestedDensity"].unique()
    )

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

    filename = (
        OUTPUT_DIR /
        f"{impl.replace(' ', '_').lower()}.png"
    )

    plt.savefig(filename)
    plt.close()

# ============================================================
# WYKRESY GĘSTOŚCI
# ============================================================

densities = sorted(
    grouped["RequestedDensity"].unique()
)

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

    filename = (
        OUTPUT_DIR /
        f"density_{density}.png"
    )

    plt.savefig(filename)
    plt.close()

print("\nWykresy wygenerowane.")