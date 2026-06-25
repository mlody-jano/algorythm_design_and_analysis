# Sorting Algorithms

## About
This project contains three implementations of **sorting algorithms** in C++, done to better understand effective sorting concepts and to compare their performance characteristics. The program enables users to choose between two modes of running:
* **Interactive menu** - Used to interactively demonstrate the behavior of each sorting algorithm and provide a user-friendly learning interface
* **Performance tester** - Strictly concentrating on testing performance of operations, enabling users to define test parameters and input data

The following sorting algorithms are implemented:
* `mergesort()` - divide-and-conquer algorithm with O(n log n) guaranteed complexity
* `quicksort()` - partition-based algorithm with O(n log n) average complexity
* `introsort()` - hybrid algorithm combining quicksort, heapsort and insertion sort

## Prerequisites
* A C++ compiler that supports the **C++17** standard (e.g., GCC, Clang, MSVC).
* **CMake** (version 3.10 or higher).

## Project report

<a href="https://github.com/mlody-jano/algorythm_design_and_analysis/blob/68fdb7b89a71d2e601edbca7e2e2288136235d5c/Project_1/PiAA_Project1.pdf" target="_blank">📄 Report</a>

## How to Build

This project uses CMake for an out-of-source build. Follow these steps to compile the code:

1. **Clone the repository** and navigate to the project directory:
   ```bash
   cd algorythm_design_and_analysis/project1
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ./project1
   ```