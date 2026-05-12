#pragma once
#include "TestDriver.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <array>

/**
 * struct BenchRow
 * structure for storing the results of a single row in the benchmark table
 * used in @class Menu for storing the results of the benchmark tests and printing them in a formatted table and exporting them to a CSV file
 */
struct BenchRow {
    size_t vertices;
    double density;
    double avgEdges;
    double avgTime;
    double minTime;
    double maxTime;
    double avgUnreachable;
};

/**
 * class Menu
 * class for running the entire program, implementing two run modes
 * - interactive mode for demonstrating the operations on the graph
 * - benchmark mode for running tests on graph implementations
 */
class Menu {
public:
    static constexpr std::array<double, 4> DENSITIES = {0.25, 0.50, 0.75, 1.00};

    void run() {

        /**
         * formatting the console output to UTF-8 to allow for printing box characters of the tables
         */
        #ifdef _WIN32
        system("chcp 65001 > nul");
        #endif
        
        printBanner_();
        bool active = true;
        while (active) {
            printMainMenu_();
            switch (readInt_<int>("Wybór: ", 0, 2)) {
                case 1: interactiveMode_();  break;
                case 2: benchmarkMode_();    break;
                case 0: active = false;      break;
            }
        }
        std::cout << "\nDo widzenia!\n";
    }

private:

    /**
     * private method for running interactive mode of the program
     * generates a random graph based on user input parameters
     * demonstrates the structure of the graph, lists the edges with weights, shows the degrees of the vertices, runs Bellman-Ford algorithm and shows the results, performs some modifications on the graph and shows the modified graph
     */
    void interactiveMode_() const {
        printHeader_("TRYB INTERAKTYWNY");
        std::cout << "Demonstracja operacji na obu implementacjach grafu.\n";

        // graph parameters
        GraphType gtype = chooseGraphType_();
        int    V    = readInt_<int>   ("Liczba wierzchołków [2–15]: ", 2,    15);
        double dens = readDouble_     ("Gęstość grafu [0.0–1.0]:    ", 0.0,  1.0);
        int    minW = readInt_<int>   ("Minimalna waga krawędzi:    ", 1,    100);
        int    maxW = readInt_<int>   ("Maksymalna waga krawędzi:   ", minW, 100);

        GraphGenerator gen(static_cast<size_t>(V), dens, gtype, minW, maxW, true);
        auto graph = gen.generate();

        // printing the generated graph
        printSection_("Wygenerowany graf");
        std::cout << "  Wierzchołki: " << graph->vertexCount()
                  << "   Krawędzie: "  << graph->edgeCount() << "\n\n";
        graph->print();

        // printing the edges with weights
        printSection_("Krawędzie z wagami");
        for (const auto& e : graph->edges())
            std::cout << "  V" << e->getFrom()
                      << " <-> V" << e->getTo()
                      << "  (waga: " << e->getWeight() << ")\n";

        // printing the degrees of the vertices
        printSection_("Stopnie wierzchołków");
        for (const auto& v : graph->vertices())
            std::cout << "  " << v->getName()
                      << " : stopień = "
                      << graph->neighbors(v->getID()).size() << "\n";

        // printing the results of Bellman-Ford algorithm
        printSection_("Bellman-Ford — najkrótsze ścieżki");
        VertexID src = graph->vertices().front()->getID();
        std::cout << "  Źródło: V" << src << "\n\n";

        auto bf = bellmanFord(*graph, src);

        if (bf.hasNegativeCycle) { std::cout << "  UWAGA: wykryto ujemny cykl!\n";} 
        else {
            std::cout << std::left
                      << std::setw(12) << "  Cel"
                      << std::setw(12) << "Odległość"
                      << "Ścieżka\n  "
                      << std::string(50, '-') << "\n";

            for (const auto& v : graph->vertices()) {
                VertexID vid = v->getID();
                if (vid == src) { continue;}
                std::cout << "  " << std::setw(10) << v->getName();

                if (!bf.reachable(vid)) { std::cout << std::setw(12) << "INF" << "(nieosiągalny)\n"; continue;}
                std::cout << std::setw(12) << bf.dist.at(vid);
                size_t i = 0;
                for (const auto& p : bf.pathTo(vid)) { std::cout << (i++ ? " -> " : "") << "V" << p;}
                std::cout << "\n";
            }
        }

        // modifing operations on the graph
        printSection_("Operacje modyfikujące");

        // deleting an edge 
        if (graph->edgeCount() > 0) {
            auto e = graph->edges().front();
            std::cout << "  Usuwanie krawędzi V" << e->getFrom()
                      << " <-> V" << e->getTo() << "...\n";
            graph->eraseEdge(e->getFrom(), e->getTo());
            std::cout << "  Krawędzi po usunięciu: " << graph->edgeCount() << "\n";
        }

        // deleting a vertex
        if (graph->vertexCount() > 2) {
            VertexID last = graph->vertices().back()->getID();
            std::cout << "  Usuwanie wierzchołka V" << last << "...\n";
            graph->eraseVertex(last);
            std::cout << "  Wierzchołków po usunięciu: "
                      << graph->vertexCount() << "\n";
        }

        // printing the modified graph
        printSection_("Graf po modyfikacjach");
        graph->print();

        pauseForUser_();
    }

    /**
     * private method for running benchmark mode of the program
     * runs tests on both graph implementations for different vertex counts and densities, collects the results, prints a summary table and exports the results to a CSV file
     */
    void benchmarkMode_() const {
        printHeader_("TRYB BENCHMARKOWY — Bellman-Ford");
        std::cout << "Stałe gęstości: 25% | 50% | 75% | 100%\n";

        // Parametry
        auto   vertexCounts = readVertexCounts_();
        int    repeats      = readInt_<int>("Powtórzenia na konfigurację: ", 1, 500);
        int    minW         = readInt_<int>("Minimalna waga krawędzi:     ", 1, 1000);
        int    maxW         = readInt_<int>("Maksymalna waga krawędzi:    ", minW, 1000);
        GraphType gtype     = chooseGraphType_();

        std::string csvPath;
        std::cout << "Nazwa pliku CSV (np. bench.csv): ";
        std::cin >> csvPath;

        std::ofstream csvFile(csvPath);
        if (!csvFile.is_open()) { std::cerr << "\nBłąd: nie można otworzyć pliku " << csvPath << "\n"; return;}
        writeCsvHeader_(csvFile);

        
        const size_t      totalConfigs = vertexCounts.size() * DENSITIES.size();
        size_t            configDone   = 0;
        std::vector<BenchRow> summary;

        // main test loop - iterating through vertex counts, densities and running tests for each configuration
        for (size_t V : vertexCounts) {
            for (double dens : DENSITIES) {
                ++configDone;
                std::cout << "\n── Konfiguracja " << configDone
                          << "/" << totalConfigs
                          << "  |V|=" << V
                          << "  gęstość=" << static_cast<int>(dens * 100) << "%\n";

                GraphGenerator gen(V, dens, gtype, minW, maxW, true);
                TestDriver     driver(gen, repeats);
                auto           results = driver.runSilent();

                // aggregating results for summary and CSV output
                double sumTime = 0, minT = 1e18, maxT = 0;
                double sumEdges = 0, sumUnreach = 0;
                for (const auto& r : results) {
                    writeCsvRow_(csvFile, r);
                    sumTime    += r.timeMs;
                    minT        = std::min(minT,  r.timeMs);
                    maxT        = std::max(maxT,  r.timeMs);
                    sumEdges   += static_cast<double>(r.edgeCount);
                    sumUnreach += r.unreachable;
                }
                double n = static_cast<double>(repeats);
                summary.push_back({V, dens,
                    sumEdges/n, sumTime/n, minT, maxT, sumUnreach/n});

                std::cout << std::fixed << std::setprecision(4)
                          << "  avg krawędzi: "    << sumEdges/n
                          << "   czas avg/min/max: "
                          << sumTime/n << " / " << minT << " / " << maxT
                          << " ms\n";
            }
        }

        csvFile.close();
        printBenchSummary_(summary, graphTypeName(gtype));
        std::cout << "\nWyniki zapisane do: " << csvPath << "\n";
    }

    // CSV functions

    /**
     * private method for writing the header row of the benchmark results CSV file
     * @param f the output file stream to write to
     */
    static void writeCsvHeader_(std::ofstream& f) {
        f << "Run,GraphType,Vertices,Edges,RequestedDensity,ActualDensity,"
          << "Seed,SourceVertex,MinDist,MaxDist,AvgDist,"
          << "UnreachableVertices,NegativeCycle,TimeMs\n";
    }

    /**
     * private method for writing a single row of the benchmark results to the CSV file
     * @param f the output file stream to write to
     * @param r the benchmark result to write as a row in the CSV file
     * formats the benchmark result into a CSV row, handling special cases such as unreachable vertices and negative cycles, and writes it to the provided file stream
     */
    static void writeCsvRow_(std::ofstream& f, const TestResult& r) {
        const bool allUnreach = (r.unreachable == (int)r.vertexCount - 1);
        std::ostringstream avgSS;
        avgSS << std::fixed << std::setprecision(4) << r.avgDist;

        f << std::fixed << std::setprecision(4)
          << r.runIndex         << "," << r.graphType        << ","
          << r.vertexCount      << "," << r.edgeCount        << ","
          << r.requestedDensity << "," << r.actualDensity    << ","
          << r.seed             << "," << "V" << r.sourceVertex << ","
          << (allUnreach ? "N/A" : std::to_string(r.minDist)) << ","
          << (allUnreach ? "N/A" : std::to_string(r.maxDist)) << ","
          << (allUnreach ? "N/A" : avgSS.str())               << ","
          << r.unreachable << ","
          << (r.negativeCycle ? "true" : "false") << ","
          << r.timeMs << "\n";
    }

    /**
     * Prints a summary table of the benchmark results.
     * @param rows The benchmark results to display.
     * @param typeName The type name of the graph being benchmarked.
     */
    static void printBenchSummary_(const std::vector<BenchRow>& rows,
                                   const std::string& typeName) {
        printHeader_("TABELA WYNIKÓW — " + typeName);
        std::cout << std::left
                  << std::setw(10) << "|V|"
                  << std::setw(12) << "Gestosc%"
                  << std::setw(12) << "Avg |E|"
                  << std::setw(14) << "Czas avg[ms]"
                  << std::setw(14) << "Czas min[ms]"
                  << std::setw(14) << "Czas max[ms]"
                  << "\n" << std::string(76, '=') << "\n";

        size_t lastV = 0;
        for (const auto& r : rows) {
            if (lastV != 0 && r.vertices != lastV)
                std::cout << std::string(76, '-') << "\n";
            lastV = r.vertices;
            std::cout << std::fixed << std::setprecision(4)
                      << std::setw(10) << r.vertices
                      << std::setw(12) << r.density * 100
                      << std::setw(12) << r.avgEdges
                      << std::setw(14) << r.avgTime
                      << std::setw(14) << r.minTime
                      << std::setw(14) << r.maxTime
                      << "\n";
        }
        std::cout << std::string(76, '=') << "\n";
    }

    /**
     * private method for reading the vertex counts for the benchmark tests from user input
     * prompts the user to enter vertex counts separated by spaces, validates the input and returns a vector of valid vertex counts (at least 2)
     */
    static std::vector<size_t> readVertexCounts_() {
        std::cout << "Podaj liczby wierzchołków oddzielone spacją\n"
                  << "(np. 10 50 100 500), zatwierdź Enterem: ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string line;
        std::getline(std::cin, line);
        std::istringstream ss(line);
        std::vector<size_t> counts;
        size_t v;
        while (ss >> v) {if (v >= 2) counts.push_back(v);};
        if (counts.empty()) {
            std::cout << "Brak poprawnych wartości — używam: 10 50 100\n";
            counts = {10, 50, 100};
        }
        return counts;
    }

    /**
     * private method for printing the program banner at the start of the program
     */
    static void printBanner_() {
        std::cout
            << "\n  ╔════════════════════════════════════════╗\n"
            << "  ║         GRAPH ADT — DEMO & BENCH       ║\n"
            << "  ║   Bellman-Ford | Lista | Macierz       ║\n"
            << "  ╚════════════════════════════════════════╝\n\n";
    }

    /**
     * private method for printing the main menu of the program
     */
    static void printMainMenu_() {
        std::cout << "\n┌─────────────────────────────────┐\n"
                  << "│          MENU GŁÓWNE             │\n"
                  << "├─────────────────────────────────┤\n"
                  << "│  1. Tryb interaktywny            │\n"
                  << "│  2. Testy wydajnościowe (bench)  │\n"
                  << "│  0. Wyjście                      │\n"
                  << "└─────────────────────────────────┘\n";
    }

    static void printHeader_(const std::string& t) {
        std::cout << "\n╔══ " << t << "\n";
    }

    static void printSection_(const std::string& t) {
        std::cout << "\n── " << t << "\n";
    }

    /**
     * private method for choosing the graph type (adjacency list or adjacency matrix) based on user input
     * prompts the user to choose the graph type, validates the input and returns the corresponding GraphType enum value
     */
    static GraphType chooseGraphType_() {
        std::cout << "\nTyp implementacji:\n"
                  << "  1 - Lista sąsiedztwa\n"
                  << "  2 - Macierz sąsiedztwa\n";
        return readInt_<int>("Wybór [1/2]: ", 1, 2) == 1
               ? GraphType::AdjacencyList
               : GraphType::AdjacencyMatrix;
    }

    /**
     * private method for pausing the program and waiting for the user to press Enter before returning to the main menu
     * utility function
     */
    static void pauseForUser_() {
        std::cout << "\nNaciśnij Enter, aby wrócić do menu...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }

    /**
     * template method for reading an integer value from user input with validation
     * used in entering various parameters in the interactive and benchmark modes ex. vertex count, edge weight range, number of repeats etc.
     */
    template<typename T>
    static T readInt_(const std::string& prompt, T lo, T hi) {
        T val;
        while (true) {
            std::cout << prompt;
            if (!std::cin.good()) {return lo;}
            if (std::cin >> val && val >= lo && val <= hi) {return val;}
            std::cout << "  Zakres: [" << lo << ", " << hi << "]\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    static double readDouble_(const std::string& prompt, double lo, double hi) {
        double val;
        while (true) {
            if (!std::cin.good()) {return lo;}
            std::cout << prompt;
            if (std::cin >> val && val >= lo && val <= hi) {return val;}
            std::cout << "  Zakres: [" << lo << ", " << hi << "]\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
};