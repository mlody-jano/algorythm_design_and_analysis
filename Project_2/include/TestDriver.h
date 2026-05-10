#pragma once
#include "GraphGenerator.h"
#include "BellmanFord.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>

using Clock = std::chrono::high_resolution_clock;
using Ms    = std::chrono::duration<double, std::milli>;

// ─────────────────────────────────────────────────────────────
//  Wyniki pojedynczego uruchomienia
// ─────────────────────────────────────────────────────────────
struct TestResult {
    int         runIndex;
    std::string graphType;
    size_t      vertexCount;
    size_t      edgeCount;
    double      requestedDensity;
    double      actualDensity;
    unsigned    seed;
    VertexID    sourceVertex;
    int         minDist;
    int         maxDist;
    double      avgDist;
    int         unreachable;
    bool        negativeCycle;
    double      timeMs;
};

// ─────────────────────────────────────────────────────────────
//  TestDriver — konfiguracja, uruchamianie i eksport wyników
// ─────────────────────────────────────────────────────────────
class TestDriver {
public:
    TestDriver(GraphGenerator generator,
               int            runs,
               std::string    csvPath = "")
        : gen_{std::move(generator)}
        , runs_{runs}
        , csvPath_{std::move(csvPath)}
    {
        if (runs_ <= 0)
            throw std::invalid_argument("Liczba uruchomień musi być > 0");
    }

    // ── Pełne uruchomienie z wydrukiem i zapisem CSV ──
    void run() {
        results_ = collectResults_();
        if (!csvPath_.empty()) saveCsv_();
        printSummary_();
        if (!csvPath_.empty())
            std::cout << "\nWyniki zapisane do: " << csvPath_ << "\n";
    }

    // ── Ciche uruchomienie — tylko zwraca wyniki ──────
    std::vector<TestResult> runSilent() {
        results_ = collectResults_();
        return results_;
    }

    const std::vector<TestResult>& results() const { return results_; }
    int                            runs()    const { return runs_; }

private:
    GraphGenerator          gen_;
    int                     runs_;
    std::string             csvPath_;
    std::vector<TestResult> results_;

    // ── Zbieranie wyników ────────────────────────
    std::vector<TestResult> collectResults_() {
        std::vector<TestResult> out;
        out.reserve(runs_);
        for (int i = 1; i <= runs_; ++i) {
            out.push_back(runSingle_(i));
            printProgress_(i);
        }
        std::cout << "\n";
        return out;
    }

    // ── Pojedynczy test ──────────────────────────
    TestResult runSingle_(int index) {
        unsigned seed = static_cast<unsigned>(
            Clock::now().time_since_epoch().count()
            + static_cast<unsigned>(index) * 2654435761u
        );
        gen_.reseed(seed);
        auto graph = gen_.generate();

        const size_t V       = graph->vertexCount();
        const size_t E       = graph->edgeCount();
        const size_t maxEdge = V * (V - 1) / 2;
        const double actDens = maxEdge > 0
                             ? static_cast<double>(E) / maxEdge : 0.0;

        VertexID source = graph->vertices().front()->getID();

        auto   t0     = Clock::now();
        auto   bf     = bellmanFord(*graph, source);
        double timeMs = Ms(Clock::now() - t0).count();

        int minD = BFResult::INF, maxD = 0;
        double sumD = 0.0;
        int reachable = 0, unreachable = 0;

        for (const auto& v : graph->vertices()) {
            if (v->getID() == source) continue;
            if (bf.reachable(v->getID())) {
                int d = bf.dist.at(v->getID());
                minD = std::min(minD, d);
                maxD = std::max(maxD, d);
                sumD += d;
                ++reachable;
            } else { ++unreachable; }
        }

        return TestResult{
            index, graphTypeName(gen_.type()),
            V, E,
            gen_.density(), actDens, seed, source,
            (minD == BFResult::INF ? 0 : minD), maxD,
            (reachable > 0 ? sumD / reachable : 0.0),
            unreachable, bf.hasNegativeCycle, timeMs
        };
    }

    // ── Zapis CSV ────────────────────────────────
    void saveCsv_() const {
        std::ofstream file(csvPath_);
        if (!file.is_open())
            throw std::runtime_error("Nie można otworzyć: " + csvPath_);
        writeHeader_(file);
        for (const auto& r : results_) writeRow_(file, r);
    }

    static void writeHeader_(std::ofstream& f) {
        f << "Run,GraphType,Vertices,Edges,RequestedDensity,ActualDensity,"
          << "Seed,SourceVertex,MinDist,MaxDist,AvgDist,"
          << "UnreachableVertices,NegativeCycle,TimeMs\n";
    }

    static void writeRow_(std::ofstream& f, const TestResult& r) {
        const bool allUnreach = (r.unreachable == (int)r.vertexCount - 1);
        f << std::fixed << std::setprecision(4)
          << r.runIndex         << "," << r.graphType        << ","
          << r.vertexCount      << "," << r.edgeCount        << ","
          << r.requestedDensity << "," << r.actualDensity    << ","
          << r.seed             << "," << "V" << r.sourceVertex << ","
          << (allUnreach ? "N/A" : std::to_string(r.minDist)) << ","
          << (allUnreach ? "N/A" : std::to_string(r.maxDist)) << ","
          << (allUnreach ? "N/A" : [&]{ std::ostringstream ss;
                ss << std::fixed << std::setprecision(4) << r.avgDist;
                return ss.str(); }()) << ","
          << r.unreachable << ","
          << (r.negativeCycle ? "true" : "false") << ","
          << r.timeMs << "\n";
    }

    // ── Pasek postępu ────────────────────────────
    void printProgress_(int cur) const {
        const int W = 25;
        int filled  = static_cast<int>(static_cast<float>(cur) / runs_ * W);
        std::cout << "\r  [";
        for (int i = 0; i < W; ++i) std::cout << (i < filled ? '#' : '-');
        std::cout << "] " << cur << "/" << runs_ << std::flush;
    }

    // ── Podsumowanie w konsoli ───────────────────
    void printSummary_() const {
        std::cout << "\n" << std::string(84, '=') << "\n"
                  << "  PODSUMOWANIE\n"
                  << std::string(84, '=') << "\n";
        std::cout << std::left << std::fixed << std::setprecision(3)
                  << std::setw(5)  << "Run"
                  << std::setw(16) << "Typ"
                  << std::setw(5)  << "|V|"
                  << std::setw(5)  << "|E|"
                  << std::setw(9)  << "Gest.%"
                  << std::setw(8)  << "MinD"
                  << std::setw(8)  << "MaxD"
                  << std::setw(10) << "AvgD"
                  << std::setw(8)  << "Nieosig"
                  << std::setw(10) << "Czas[ms]"
                  << "\n" << std::string(84, '-') << "\n";

        for (const auto& r : results_)
            std::cout << std::setw(5)  << r.runIndex
                      << std::setw(16) << r.graphType
                      << std::setw(5)  << r.vertexCount
                      << std::setw(5)  << r.edgeCount
                      << std::setw(9)  << r.actualDensity * 100
                      << std::setw(8)  << r.minDist
                      << std::setw(8)  << r.maxDist
                      << std::setw(10) << r.avgDist
                      << std::setw(8)  << r.unreachable
                      << std::setw(10) << r.timeMs << "\n";

        double tot = std::accumulate(results_.begin(), results_.end(), 0.0,
            [](double s, const TestResult& r){ return s + r.timeMs; });
        double mn  = std::min_element(results_.begin(), results_.end(),
            [](auto& a, auto& b){ return a.timeMs < b.timeMs; })->timeMs;
        double mx  = std::max_element(results_.begin(), results_.end(),
            [](auto& a, auto& b){ return a.timeMs < b.timeMs; })->timeMs;

        std::cout << std::string(84, '-') << "\n" << std::setprecision(4)
                  << "  Czas min/avg/max: "
                  << mn << " / " << tot/results_.size() << " / " << mx
                  << " ms    Łącznie: " << tot << " ms\n";
    }
};