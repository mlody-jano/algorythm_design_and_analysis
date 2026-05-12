#pragma once
#include "Graph.h"
#include <unordered_map>
#include <limits>
#include <string>
#include <vector>
#include <stdexcept>

/**
 * struct BFResult
 * structure for storing the result of Bellman-Ford algorithm
 */
struct BFResult {
    // dist[v]  => shortest distance from source to v; INF if unreachable
    std::unordered_map<VertexID, int>      dist;

    // prev[v]  => previous vertex on the shortest path from source to v; NO_PREV if unreachable or v is source
    std::unordered_map<VertexID, VertexID> prev;

    bool hasNegativeCycle = false;

    static constexpr int      INF     = std::numeric_limits<int>::max() / 2;
    static constexpr VertexID NO_PREV = std::numeric_limits<VertexID>::max();

    bool reachable(VertexID v) const {
        auto it = dist.find(v);
        return it != dist.end() && it->second < INF;
    }

    // Reconstructs the shortest path from source to v as a vector of vertex IDs
    std::vector<VertexID> pathTo(VertexID v) const {
        if (!reachable(v)) return {};
        std::vector<VertexID> path;
        for (VertexID cur = v; cur != NO_PREV; cur = prev.at(cur))
            path.push_back(cur);
        std::reverse(path.begin(), path.end());
        return path;
    }
};

/**
 * function bellmanFord
 * implementation of Bellman-Ford algorithm for finding the shortest paths from a source vertex to all other vertices in a graph
 * @param g graph to run the algorithm on, @param source ID of the source vertex
 * @return BFResult structure containing the distances, previous vertices, and negative cycle flag
 * throws VertexNotFoundException if the source vertex is not found in the graph
 */
template <typename V>
BFResult bellmanFord(const Graph<V, int>& g, VertexID source) {
    if (!g.hasVertex(source)) {throw VertexNotFoundException(source);}

    BFResult result;
    const int      INF      = BFResult::INF;
    const VertexID NO_PREV  = BFResult::NO_PREV;

    // Inicjalizacja — wszystkie odległości = INF, źródło = 0
    for (const auto& v : g.vertices()) {
        result.dist[v->getID()] = INF;
        result.prev[v->getID()] = NO_PREV;
    }
    result.dist[source] = 0;

    const size_t numV      = g.vertexCount();
    const bool   directed  = g.isDirected();

    // main loop -> iterating |V|-1 times, relaxing all edges
    for (size_t iter = 0; iter < numV - 1; ++iter) {
        bool updated = false;

        g.forEachEdge([&](VertexID u, VertexID v, int w) {
            if (result.dist[u] < INF &&
                result.dist[u] + w < result.dist[v]) // if the path through u is shorter than the previously known path to v -> update
            {
                result.dist[v] = result.dist[u] + w;
                result.prev[v] = u;
                updated = true;
            }
            if (!directed &&
                result.dist[v] < INF &&
                result.dist[v] + w < result.dist[u]) // if the path through v is shorter than the previously known path to u -> update (for undirected graphs)
            {
                result.dist[u] = result.dist[v] + w;
                result.prev[u] = v;
                updated = true;
            }
        });

        if (!updated) break;                        // early stop if no updates were made
    }

    // negative cycle detection
    g.forEachEdge([&](VertexID u, VertexID v, int w) {
        if (result.hasNegativeCycle) { return;}
        if ((result.dist[u] < INF && result.dist[u] + w < result.dist[v]) ||
            (!directed && result.dist[v] < INF && result.dist[v] + w < result.dist[u])) { result.hasNegativeCycle = true;} // if we can still relax an edge after |V|-1 iterations, then there is a negative cycle in the graph
    });

    return result;
}