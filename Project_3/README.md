# Tic-Tac-Toe — `minimax()` with Alpha-Beta Pruning

## About
This project contains an implementation of a **Tic-Tac-Toe game** in C++ with a computer-controlled AI opponent. The program enables users to choose between two modes of running:
* **Interactive game** - Used to play against the AI player in a user-friendly graphical interface built with SFML

The AI opponent is powered by the `minimax()` algorithm with **alpha-beta pruning**, which significantly reduces the number of nodes evaluated in the game tree. Users can specify:
* **Board size** - defines the dimensions of the game board
* **Search depth** - controls how many moves ahead the AI looks, allowing tweaks in difficulty and performance

## Prerequisites
* A C++ compiler that supports the **C++17** standard (e.g., GCC, Clang, MSVC).
* **CMake** (version 3.10 or higher).
* **SFML** (version 3.x) — used for graphical rendering of the game board.

## How to Build

This project uses CMake for an out-of-source build. Follow these steps to compile the code:

1. **Clone the repository** and navigate to the project directory:
   ```bash
   cd algorythm_design_and_analysis/project3
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ./project3
   ```