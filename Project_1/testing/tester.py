import subprocess
import os
import sys
import numpy as np
import matplotlib.pyplot as plt
import csv

# Parametry testu
executable_path             = "build/Project_1"                              # Path to the compiled C++ executable
data_file                   = f"./data/project_data.csv"                      # Path to the input data file (CSV with 1 million entries)                  
sizes_to_test               = [10000, 100000, 500000, 1000000]                # Sizes of the data structure to test (10K, 100K, 500K, 1M)
algorythms                  = ['merge', 'quick', 'intro']                     # Sorting algorithms to test
repetitions                 = 10                                              # Number of repetitions for each test

# CSV output file
csv_output_path             = "benchmark_results.csv"
csv_columns                 = ["algorithm", "size", "repetition", "loading_time_ns", "sorting_time_ns", "average_value", "median_value"]

# Lists for saving results
sizes_plot = np.array(sizes_to_test)
sorting_time_qso, sorting_time_mso, sorting_time_iso = [], [], []
loading_time_qso, loading_time_mso, loading_time_iso = [], [], []

average_val_qso, median_val_qso = [], []
average_val_mso, median_val_mso = [], []
average_val_iso, median_val_iso = [], []

with open(csv_output_path, mode='w', newline='') as csv_file:
    writer = csv.DictWriter(csv_file, fieldnames=csv_columns)
    writer.writeheader()

    for algo in algorythms:
        for i, size in enumerate(sizes_to_test):
            avg_results = np.zeros((4, repetitions), dtype=float)  # 4 values: loading, sorting, average, median
            for rep in range(repetitions):
                result = subprocess.run(
                    [executable_path, str(algo), str(size), data_file],
                    capture_output=True, text=True, errors='replace'
                )

                if result.returncode == 0:
                    output = result.stdout.strip().split(',')
                    avg_results[0, rep] = float(output[0])  # loading time
                    avg_results[1, rep] = float(output[1])  # sorting time
                    avg_results[2, rep] = float(output[2])  # average value
                    avg_results[3, rep] = float(output[3])  # median value

                    print(
                        f"[{algo}] Size {size}, rep {rep+1}/{repetitions}: "
                        f"Load = {avg_results[0, rep]:.0f} ns, "
                        f"Sort = {avg_results[1, rep]:.0f} ns, "
                        f"Avg = {avg_results[2, rep]}, "
                        f"Median = {avg_results[3, rep]}"
                    )

                    writer.writerow({
                        "algorithm":       algo,
                        "size":            size,
                        "repetition":      rep + 1,
                        "loading_time_ns": avg_results[0, rep],
                        "sorting_time_ns": avg_results[1, rep],
                        "average_value":   avg_results[2, rep],
                        "median_value":    avg_results[3, rep],
                    })
                else:
                    print(f"Error for [{algo}] size {size}: {result.stderr}")
                    break

            means = np.mean(avg_results, axis=1)

            if algo == 'quick':
                loading_time_qso.append(means[0])
                sorting_time_qso.append(means[1])
                average_val_qso.append(means[2])
                median_val_qso.append(means[3])
            elif algo == 'merge':
                loading_time_mso.append(means[0])
                sorting_time_mso.append(means[1])
                average_val_mso.append(means[2])
                median_val_mso.append(means[3])
            elif algo == 'intro':
                loading_time_iso.append(means[0])
                sorting_time_iso.append(means[1])
                average_val_iso.append(means[2])
                median_val_iso.append(means[3])

print(f"\nResults saved to '{csv_output_path}'.")

# Aggregate average/median across all sizes
average_val_qso = np.mean(average_val_qso)
median_val_qso  = np.mean(median_val_qso)
average_val_mso = np.mean(average_val_mso)
median_val_mso  = np.mean(median_val_mso)
average_val_iso = np.mean(average_val_iso)
median_val_iso  = np.mean(median_val_iso)


# --- Helper to apply common plot formatting ---
def setup_plot(title):
    plt.figure(figsize=(12, 7))
    plt.title(title, fontsize=14)
    plt.xlabel('Structure Size (N)', fontsize=12)
    plt.ylabel('Execution Time [nanoseconds]', fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)
    ax = plt.gca()
    ax.get_xaxis().set_major_formatter(plt.FuncFormatter(lambda x, p: format(int(x), ',')))
    ax.get_yaxis().set_major_formatter(plt.FuncFormatter(lambda y, p: format(int(y), ',')))
    return ax


# --- QuickSort plot ---
print("\nGenerating plots...")

setup_plot('Time Complexity of QuickSort Algorithm')
plt.plot(sizes_plot, sorting_time_qso, label='Quick Sort - Sorting',  marker='o', linewidth=2)
plt.legend(fontsize=11)
plt.tight_layout()
plt.savefig('complexity_results_quick.png', dpi=300, bbox_inches='tight')
print(f"Quick Sort — average value: {average_val_qso:.4f}, median value: {median_val_qso:.4f}")
print("Plot saved as 'complexity_results_quick.png'.")
plt.clf()

# --- MergeSort plot ---
setup_plot('Time Complexity of MergeSort Algorithm')
plt.plot(sizes_plot, sorting_time_mso, label='Merge Sort - Sorting', marker='s', linewidth=2)
plt.legend(fontsize=11)
plt.tight_layout()
plt.savefig('complexity_results_merge.png', dpi=300, bbox_inches='tight')
print(f"Merge Sort — average value: {average_val_mso:.4f}, median value: {median_val_mso:.4f}")
print("Plot saved as 'complexity_results_merge.png'.")
plt.clf()

# --- IntroSort plot ---
setup_plot('Time Complexity of IntroSort Algorithm')
plt.plot(sizes_plot, sorting_time_iso, label='Intro Sort - Sorting', marker='^', linewidth=2)
plt.legend(fontsize=11)
plt.tight_layout()
plt.savefig('complexity_results_intro.png', dpi=300, bbox_inches='tight')
print(f"Intro Sort — average value: {average_val_iso:.4f}, median value: {median_val_iso:.4f}")
print("Plot saved as 'complexity_results_intro.png'.")
plt.clf()

# --- Loading plot ---
setup_plot('Time Complexity of Loading Data')
plt.plot(sizes_plot, loading_time_iso, label='Loading', marker='^', linewidth=2)
plt.legend(fontsize=11)
plt.tight_layout()
plt.savefig('loading_results.png', dpi=300, bbox_inches='tight')
print("Plot saved as 'loading_results.png'.")
plt.clf()

# --- Combined plot ---
setup_plot('Time Complexity of Sorting Algorithms')
plt.plot(sizes_plot, sorting_time_qso, label='Quick Sort - Sorting',  marker='o', linewidth=2)
plt.plot(sizes_plot, sorting_time_mso, label='Merge Sort - Sorting', marker='s', linewidth=2)
plt.plot(sizes_plot, sorting_time_iso, label='Intro Sort - Sorting', marker='^', linewidth=2)
plt.legend(fontsize=11)
plt.tight_layout()
plt.savefig('complexity_results_combined.png', dpi=300, bbox_inches='tight')
print("Combined plot saved as 'complexity_results_combined.png'.")
plt.clf()
