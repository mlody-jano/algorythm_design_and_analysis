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

/**
 * struct TestResult
 * structure for storing the results of a single test run of the Bellman-Ford algorithm on a generated graph
 * used in @class TestDriver for storing the results of each test run and printing them in a formatted summary and exporting them to a CSV file
 */
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

/**
 * class TestDriver
 * class for running multiple tests of the Bellman-Ford algorithm on randomly generated graphs and collecting the results
 * allows for running a specified number of tests, printing a summary of the results, and exporting the results to a CSV file for further analysis
 * 
 */
class TestDriver {
public:

    /**
     * Constructor for the TestDriver class
     * @param generator The graph generator to use for creating test graphs
     * @param runs The number of test runs to perform
     * @param csvPath The path to the CSV file where results will be saved (optional)
     */
    TestDriver(GraphGenerator generator,
               int            runs,
               std::string    csvPath = "")
        : gen_{std::move(generator)}
        , runs_{runs}
        , csvPath_{std::move(csvPath)}
    { if (runs_ <= 0) { throw std::invalid_argument("Liczba uruchomień musi być > 0");}}

    /**
     * method for running the tests and collecting the results
     * runs the specified number of tests, collects the results in a vector of TestResult structures, saves the results to a CSV file if a path was provided, and prints a summary of the results to the console
     */
    void run() {
        results_ = collectResults_();
        if (!csvPath_.empty()) { saveCsv_();}
        printSummary_();
        if (!csvPath_.empty()) {std::cout << "\nWyniki zapisane do: " << csvPath_ << "\n";}
    }

    /**
     * method for running the tests silently and returning the results
     * runs the specified number of tests, collects the results in a vector of TestResult structures, and returns the vector without saving to a CSV file or printing a summary
     */
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

    /**
     * method for collecting the results of all test runs
     * runs the specified number of tests and collects the results in a vector of TestResult structures
     */
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

    /**
     * method for running a single test
     * @param index The index of the test run
     * @return The result of the test run
     */
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
            if (v->getID() == source) { continue;}
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

    /**
     * method for saving the results to a CSV file
     * @param file The output file stream
     */
    void saveCsv_() const {
        std::ofstream file(csvPath_);
        if (!file.is_open()) { throw std::runtime_error("Nie można otworzyć: " + csvPath_);}
        writeHeader_(file);
        for (const auto& r : results_) {writeRow_(file, r);};
    }

    /**
     * private method for writing the header row of the CSV file
     * @param f the output file stream to write to
     */
    static void writeHeader_(std::ofstream& f) {
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

    /**
     * private method for printing the progress of the test runs in the console
     * @param cur the current test run index
     * utility function for providing feedback to the user about the progress of the test runs
     */
    void printProgress_(int cur) const {
        const int W = 25;
        int filled  = static_cast<int>(static_cast<float>(cur) / runs_ * W);
        std::cout << "\r  [";
        for (int i = 0; i < W; ++i) {std::cout << (i < filled ? '#' : '-');}
        std::cout << "] " << cur << "/" << runs_ << std::flush;
    }

    /**
     * private method for printing a summary of the test results to the console
     * formats the results in a table with columns for each relevant metric and prints it to the console
     */
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