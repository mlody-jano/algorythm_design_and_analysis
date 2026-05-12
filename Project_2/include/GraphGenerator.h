#pragma once
#include "AdjListGraph.h"
#include "AdjMtxGraph.h"
#include <memory>
#include <random>
#include <string>
#include <stdexcept>

/**
 * enum GraphType
 * enumerator for the type of graph to be generated: adjacency list or adjacency matrix
 * used in @class GraphGenerator to determine which type of graph to generate
 */
enum class GraphType { AdjacencyList, AdjacencyMatrix };

/**
 * function graphTypeName
 * helper function for converting a GraphType value to a string for printing purposes
 * @param t GraphType value to be converted
 * @return string representation of the GraphType value
 */
inline std::string graphTypeName(GraphType t) {
    return t == GraphType::AdjacencyList ? "AdjacencyList" : "AdjacencyMatrix";
}

/**
 * class GraphGenerator
 * class for generating random graphs of specified type, vertex count, edge density, weight range, and directedness
 */
class GraphGenerator {
public:
    /**
     * parametric constructor for GraphGenerator
     * @param vertexCount number of vertices in the generated graph
     * @param density edge density of the generated graph
     * @param type type of the generated graph (adjacency list or adjacency matrix)
     * @param minWeight minimum weight of the edges in the generated graph
     * @param maxWeight maximum weight of the edges in the generated graph
     * @param directed whether the generated graph is directed
     * @param seed seed for the random number generator
     */
    GraphGenerator(size_t    vertexCount,
                   double    density,
                   GraphType type,
                   int       minWeight = 1,
                   int       maxWeight = 20,
                   bool      directed  = false,
                   unsigned  seed      = std::random_device{}())
        : vertexCount_{vertexCount}
        , density_{density}
        , type_{type}
        , minWeight_{minWeight}
        , maxWeight_{maxWeight}
        , directed_{directed}
        , seed_{seed}
    {
        validate();
    }

    // generating a random graph based on the parameters of the generator
    std::unique_ptr<Graph<std::string, int>> generate() const {
        std::unique_ptr<Graph<std::string, int>> graph;

        if (type_ == GraphType::AdjacencyList) {graph = std::make_unique<AdjListGraph<std::string, int>>(directed_);} 
        else { graph = std::make_unique<AdjMtxGraph<std::string, int>>(directed_);}

        // Dodaj wierzchołki
        std::vector<VertexID> ids;
        ids.reserve(vertexCount_);

        for (size_t i = 0; i < vertexCount_; ++i) {ids.push_back(graph->addVertex("V" + std::to_string(i))->getID());}

        // random generator based on mersenne twister engine, seeded with the seed provided in the constructor
        std::mt19937 rng(seed_);
        std::uniform_real_distribution<double> prob(0.0, 1.0);
        std::uniform_int_distribution<int> weight(minWeight_, maxWeight_);

        // Generowanie krawędzi skierowanych bez duplikatów
        for (size_t i = 0; i < vertexCount_; ++i) {
            for (size_t j = i + 1; j < vertexCount_; ++j) {
                if (prob(rng) < density_) {
                    if (prob(rng) < 0.5) { graph->addEdge(ids[i], ids[j], weight(rng));} 
                    else { graph->addEdge(ids[j], ids[i], weight(rng));}
                }
            }
        }

        return graph;
    }

    // getters for the parameters of the generator
    size_t    vertexCount() const { return vertexCount_; }
    double    density()     const { return density_; }
    GraphType type()        const { return type_; }
    bool      directed()    const { return directed_; }
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
    bool      directed_;
    unsigned  seed_;

    /**
     * private method for validating the parameters of the generator
     * @throws std::invalid_argument if any of the parameters are invalid (e.g. vertex count is 0, density is not in [0,1], minWeight is greater than maxWeight)
     */
    void validate() const {
        if (vertexCount_ == 0) {throw std::invalid_argument("Liczba wierzchołków musi być > 0");}
        if (density_ < 0.0 || density_ > 1.0) {throw std::invalid_argument("Gęstość musi być w przedziale [0.0, 1.0]");}
        if (minWeight_ > maxWeight_) {throw std::invalid_argument("minWeight musi być <= maxWeight");}
    }
};