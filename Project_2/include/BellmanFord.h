#pragma once
#include "Graph.h"
#include <unordered_map>
#include <limits>
#include <string>
#include <vector>
#include <stdexcept>

// ─────────────────────────────────────────────────────────────
//  Wynik algorytmu Bellmana-Forda
// ─────────────────────────────────────────────────────────────
struct BFResult {
    // dist[v]  = najkrótsza odległość od źródła do v
    //            MAX jeśli wierzchołek nieosiągalny
    std::unordered_map<VertexID, int>      dist;

    // prev[v]  = poprzednik v na najkrótszej ścieżce
    //            SIZE_MAX jeśli brak poprzednika
    std::unordered_map<VertexID, VertexID> prev;

    bool hasNegativeCycle = false;

    static constexpr int      INF     = std::numeric_limits<int>::max() / 2;
    static constexpr VertexID NO_PREV = std::numeric_limits<VertexID>::max();

    bool reachable(VertexID v) const {
        auto it = dist.find(v);
        return it != dist.end() && it->second < INF;
    }

    // Odtworz ścieżkę od źródła do v
    std::vector<VertexID> pathTo(VertexID v) const {
        if (!reachable(v)) return {};
        std::vector<VertexID> path;
        for (VertexID cur = v; cur != NO_PREV; cur = prev.at(cur))
            path.push_back(cur);
        std::reverse(path.begin(), path.end());
        return path;
    }
};

// ─────────────────────────────────────────────────────────────
//  Algorytm Bellmana-Forda
//
//  Dla grafu nieskierowanego każda krawędź (u,v,w) jest
//  traktowana jako dwie krawędzie skierowane: u→v i v→u.
//  Złożoność: O(V · E)
// ─────────────────────────────────────────────────────────────
template <typename V>
BFResult bellmanFord(const Graph<V, int>& g, VertexID source) {
    if (!g.hasVertex(source))
        throw VertexNotFoundException(source);

    BFResult result;
    const int      INF     = BFResult::INF;
    const VertexID NO_PREV = BFResult::NO_PREV;

    // Inicjalizacja — wszystkie odległości = INF, źródło = 0
    for (const auto& v : g.vertices()) {
        result.dist[v->getID()] = INF;
        result.prev[v->getID()] = NO_PREV;
    }
    result.dist[source] = 0;

    const size_t numV = g.vertexCount();
    auto         allEdges = g.edges();

    // Główna pętla: V-1 relaksacji
    for (size_t iter = 0; iter < numV - 1; ++iter) {
        bool updated = false;

        for (const auto& edge : allEdges) {
            VertexID u = edge->getFrom();
            VertexID v = edge->getTo();
            int      w = edge->getWeight();

            // Relaksacja u → v
            if (result.dist[u] < INF &&
                result.dist[u] + w < result.dist[v])
            {
                result.dist[v] = result.dist[u] + w;
                result.prev[v] = u;
                updated = true;
            }

            // Relaksacja v → u (graf nieskierowany)
            if (result.dist[v] < INF &&
                result.dist[v] + w < result.dist[u])
            {
                result.dist[u] = result.dist[v] + w;
                result.prev[u] = v;
                updated = true;
            }
        }

        // Wczesne zakończenie gdy brak poprawy
        if (!updated) break;
    }

    // Detekcja ujemnych cykli — dodatkowa relaksacja
    for (const auto& edge : allEdges) {
        VertexID u = edge->getFrom();
        VertexID v = edge->getTo();
        int      w = edge->getWeight();

        if ((result.dist[u] < INF && result.dist[u] + w < result.dist[v]) ||
            (result.dist[v] < INF && result.dist[v] + w < result.dist[u]))
        {
            result.hasNegativeCycle = true;
            break;
        }
    }

    return result;
}