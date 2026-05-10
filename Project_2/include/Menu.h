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

// ─────────────────────────────────────────────────────────────
//  Wiersz tabeli podsumowującej benchmark
// ─────────────────────────────────────────────────────────────
struct BenchRow {
    size_t vertices;
    double density;
    double avgEdges;
    double avgTime;
    double minTime;
    double maxTime;
    double avgUnreachable;
};

// ─────────────────────────────────────────────────────────────
//  Menu — punkt wejścia dla użytkownika
//
//  Tryb 1 — Interaktywny : demonstracja operacji na grafach
//  Tryb 2 — Benchmarkowy : testy Bellmana-Forda dla zadanych
//            liczb wierzchołków × 4 stałe gęstości
// ─────────────────────────────────────────────────────────────
class Menu {
public:
    static constexpr std::array<double, 4> DENSITIES = {0.25, 0.50, 0.75, 1.00};

    void run() {

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

    // ════════════════════════════════════════════
    //  TRYB INTERAKTYWNY
    // ════════════════════════════════════════════
    void interactiveMode_() const {
        printHeader_("TRYB INTERAKTYWNY");
        std::cout << "Demonstracja operacji na obu implementacjach grafu.\n";

        // Parametry grafu
        GraphType gtype = chooseGraphType_();
        int    V    = readInt_<int>   ("Liczba wierzchołków [2–15]: ", 2,    15);
        double dens = readDouble_     ("Gęstość grafu [0.0–1.0]:    ", 0.0,  1.0);
        int    minW = readInt_<int>   ("Minimalna waga krawędzi:    ", 1,    100);
        int    maxW = readInt_<int>   ("Maksymalna waga krawędzi:   ", minW, 100);

        GraphGenerator gen(static_cast<size_t>(V), dens, gtype, minW, maxW);
        auto graph = gen.generate();

        // Struktura grafu
        printSection_("Wygenerowany graf");
        std::cout << "  Wierzchołki: " << graph->vertexCount()
                  << "   Krawędzie: "  << graph->edgeCount() << "\n\n";
        graph->print();

        // Lista krawędzi z wagami
        printSection_("Krawędzie z wagami");
        for (const auto& e : graph->edges())
            std::cout << "  V" << e->getFrom()
                      << " <-> V" << e->getTo()
                      << "  (waga: " << e->getWeight() << ")\n";

        // Stopnie wierzchołków
        printSection_("Stopnie wierzchołków");
        for (const auto& v : graph->vertices())
            std::cout << "  " << v->getName()
                      << " : stopień = "
                      << graph->neighbors(v->getID()).size() << "\n";

        // Bellman-Ford
        printSection_("Bellman-Ford — najkrótsze ścieżki");
        VertexID src = graph->vertices().front()->getID();
        std::cout << "  Źródło: V" << src << "\n\n";

        auto bf = bellmanFord(*graph, src);

        if (bf.hasNegativeCycle) {
            std::cout << "  UWAGA: wykryto ujemny cykl!\n";
        } else {
            std::cout << std::left
                      << std::setw(12) << "  Cel"
                      << std::setw(12) << "Odległość"
                      << "Ścieżka\n  "
                      << std::string(50, '-') << "\n";

            for (const auto& v : graph->vertices()) {
                VertexID vid = v->getID();
                if (vid == src) continue;
                std::cout << "  " << std::setw(10) << v->getName();

                if (!bf.reachable(vid)) {
                    std::cout << std::setw(12) << "INF" << "(nieosiągalny)\n";
                    continue;
                }
                std::cout << std::setw(12) << bf.dist.at(vid);
                size_t i = 0;
                for (const auto& p : bf.pathTo(vid))
                    std::cout << (i++ ? " -> " : "") << "V" << p;
                std::cout << "\n";
            }
        }

        // Operacje modyfikujące
        printSection_("Operacje modyfikujące");

        if (graph->edgeCount() > 0) {
            auto e = graph->edges().front();
            std::cout << "  Usuwanie krawędzi V" << e->getFrom()
                      << " <-> V" << e->getTo() << "...\n";
            graph->eraseEdge(e->getFrom(), e->getTo());
            std::cout << "  Krawędzi po usunięciu: " << graph->edgeCount() << "\n";
        }

        if (graph->vertexCount() > 2) {
            VertexID last = graph->vertices().back()->getID();
            std::cout << "  Usuwanie wierzchołka V" << last << "...\n";
            graph->eraseVertex(last);
            std::cout << "  Wierzchołków po usunięciu: "
                      << graph->vertexCount() << "\n";
        }

        printSection_("Graf po modyfikacjach");
        graph->print();

        pauseForUser_();
    }

    // ════════════════════════════════════════════
    //  TRYB BENCHMARKOWY
    // ════════════════════════════════════════════
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
        if (!csvFile.is_open()) {
            std::cerr << "\nBłąd: nie można otworzyć pliku " << csvPath << "\n";
            return;
        }
        writeCsvHeader_(csvFile);

        // Główna pętla:  |V| × gęstość × powtórzenia
        const size_t      totalConfigs = vertexCounts.size() * DENSITIES.size();
        size_t            configDone   = 0;
        std::vector<BenchRow> summary;

        for (size_t V : vertexCounts) {
            for (double dens : DENSITIES) {
                ++configDone;
                std::cout << "\n── Konfiguracja " << configDone
                          << "/" << totalConfigs
                          << "  |V|=" << V
                          << "  gęstość=" << static_cast<int>(dens * 100) << "%\n";

                GraphGenerator gen(V, dens, gtype, minW, maxW);
                TestDriver     driver(gen, repeats);
                auto           results = driver.runSilent();

                // Agregacja dla tej konfiguracji
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

    // ─────────────────────────────────────────────────────────
    //  CSV
    // ─────────────────────────────────────────────────────────
    static void writeCsvHeader_(std::ofstream& f) {
        f << "Run,GraphType,Vertices,Edges,RequestedDensity,ActualDensity,"
          << "Seed,SourceVertex,MinDist,MaxDist,AvgDist,"
          << "UnreachableVertices,NegativeCycle,TimeMs\n";
    }

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

    // ─────────────────────────────────────────────────────────
    //  Tabela końcowa benchmarku
    // ─────────────────────────────────────────────────────────
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

    // ─────────────────────────────────────────────────────────
    //  Wczytywanie listy liczb wierzchołków
    // ─────────────────────────────────────────────────────────
    static std::vector<size_t> readVertexCounts_() {
        std::cout << "Podaj liczby wierzchołków oddzielone spacją\n"
                  << "(np. 10 50 100 500), zatwierdź Enterem: ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string line;
        std::getline(std::cin, line);
        std::istringstream ss(line);
        std::vector<size_t> counts;
        size_t v;
        while (ss >> v) if (v >= 2) counts.push_back(v);
        if (counts.empty()) {
            std::cout << "Brak poprawnych wartości — używam: 10 50 100\n";
            counts = {10, 50, 100};
        }
        return counts;
    }

    // ─────────────────────────────────────────────────────────
    //  Pomocnicze metody UI
    // ─────────────────────────────────────────────────────────
    static void printBanner_() {
        std::cout
            << "\n  ╔════════════════════════════════════════╗\n"
            << "  ║         GRAPH ADT — DEMO & BENCH       ║\n"
            << "  ║   Bellman-Ford | Lista | Macierz       ║\n"
            << "  ╚════════════════════════════════════════╝\n\n";
    }

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

    static GraphType chooseGraphType_() {
        std::cout << "\nTyp implementacji:\n"
                  << "  1 - Lista sąsiedztwa\n"
                  << "  2 - Macierz sąsiedztwa\n";
        return readInt_<int>("Wybór [1/2]: ", 1, 2) == 1
               ? GraphType::AdjacencyList
               : GraphType::AdjacencyMatrix;
    }

    static void pauseForUser_() {
        std::cout << "\nNaciśnij Enter, aby wrócić do menu...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }

    template<typename T>
    static T readInt_(const std::string& prompt, T lo, T hi) {
        T val;
        while (true) {
            std::cout << prompt;
            if (!std::cin.good()) return lo;   // EOF lub błąd — wyjdź z pętli
            if (std::cin >> val && val >= lo && val <= hi) return val;
            std::cout << "  Zakres: [" << lo << ", " << hi << "]\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    static double readDouble_(const std::string& prompt, double lo, double hi) {
        double val;
        while (true) {
            if (!std::cin.good()) return lo;
            std::cout << prompt;
            if (std::cin >> val && val >= lo && val <= hi) return val;
            std::cout << "  Zakres: [" << lo << ", " << hi << "]\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
};