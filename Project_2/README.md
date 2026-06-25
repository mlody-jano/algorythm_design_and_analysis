# Graph Algorithms — Bellman-Ford (SSSP)

## About
This project contains an implementation of the **Bellman-Ford algorithm** in C++, used to solve the **single-source shortest path problem** (SSSPP) on weighted directed graphs. The program enables users to choose between two modes of running:
* **Interactive menu** - Used to interactively demonstrate the algorithm's operation on a given graph and to provide a user-friendly learning interface
* **Performance tester** - Strictly concentrating on testing performance of operations, enabling users to define test parameters and input data

The Bellman-Ford algorithm correctly handles graphs with **negative edge weights** and is capable of detecting **negative weight cycles**.

## Prerequisites
* A C++ compiler that supports the **C++17** standard (e.g., GCC, Clang, MSVC).
* **CMake** (version 3.10 or higher).

## How to Build

This project uses CMake for an out-of-source build. Follow these steps to compile the code:

1. **Clone the repository** and navigate to the project directory:
   ```bash
   cd algorythm_design_and_analysis/project2
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ./project2
   ```