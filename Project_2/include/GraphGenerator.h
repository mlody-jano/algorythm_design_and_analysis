#pragma once
#include "AdjListGraph.h"
#include "AdjMtxGraph.h"
#include <memory>
#include <random>
#include <string>
#include <stdexcept>

enum class GraphType { AdjacencyList, AdjacencyMatrix };

inline std::string graphTypeName(GraphType t) {
    return t == GraphType::AdjacencyList ? "AdjacencyList" : "AdjacencyMatrix";
}

// ─────────────────────────────────────────────────────────────
//  GraphGenerator — tworzy losowe ważone grafy skierowane
// ─────────────────────────────────────────────────────────────
class GraphGenerator {
public:
    GraphGenerator(size_t    vertexCount,
                   double    density,
                   GraphType type,
                   int       minWeight = 1,
                   int       maxWeight = 20,
                   unsigned  seed      = std::random_device{}())
        : vertexCount_{vertexCount}
        , density_{density}
        , type_{type}
        , minWeight_{minWeight}
        , maxWeight_{maxWeight}
        , seed_{seed}
    {
        validate();
    }

    // ── Generowanie ──────────────────────────────
    std::unique_ptr<Graph<std::string, int>> generate() const {
        std::unique_ptr<Graph<std::string, int>> graph;

        if (type_ == GraphType::AdjacencyList) {
            graph = std::make_unique<AdjListGraph<std::string, int>>();
        } else {
            graph = std::make_unique<AdjMtxGraph<std::string, int>>();
        }

        // Dodaj wierzchołki
        std::vector<VertexID> ids;
        ids.reserve(vertexCount_);

        for (size_t i = 0; i < vertexCount_; ++i)
            ids.push_back(graph->addVertex("V" + std::to_string(i))->getID());

        // Generator losowy
        std::mt19937 rng(seed_);
        std::uniform_real_distribution<double> prob(0.0, 1.0);
        std::uniform_int_distribution<int> weight(minWeight_, maxWeight_);

        // Generowanie krawędzi skierowanych
        for (size_t i = 0; i < vertexCount_; ++i) {
            for (size_t j = 0; j < vertexCount_; ++j) {

                // Pomijamy pętle i -> i
                if (i == j)
                    continue;

                // Losujemy niezależnie krawędź i -> j
                if (prob(rng) < density_) {
                    graph->addEdge(ids[i], ids[j], weight(rng));
                }
            }
        }

        return graph;
    }

    // ── Gettery ──────────────────────────────────
    size_t    vertexCount() const { return vertexCount_; }
    double    density()     const { return density_; }
    GraphType type()        const { return type_; }
    int       minWeight()   const { return minWeight_; }
    int       maxWeight()   const { return maxWeight_; }
    unsigned  seed()        const { return seed_; }

    void reseed(unsigned newSeed) { seed_ = newSeed; }

private:
    size_t    vertexCount_;
    double    density_;
    GraphType type_;
    int       minWeight_;
    int       maxWeight_;
    unsigned  seed_;

    void validate() const {
        if (vertexCount_ == 0)
            throw std::invalid_argument("Liczba wierzchołków musi być > 0");

        if (density_ < 0.0 || density_ > 1.0)
            throw std::invalid_argument("Gęstość musi być w przedziale [0.0, 1.0]");

        if (minWeight_ > maxWeight_)
            throw std::invalid_argument("minWeight musi być <= maxWeight");
    }
};