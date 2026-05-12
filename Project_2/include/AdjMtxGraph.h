#pragma once
#include "Graph.h"
#include <vector>
#include <unordered_map>
#include <limits>
#include <iostream>

template <typename V, typename E>
class AdjMtxGraph : public Graph<V,E> {
public:
    using VertexPtr = std::shared_ptr<Vertex<V,E>>;
    using EdgePtr   = std::shared_ptr<Edge<E>>;

    static constexpr E NO_EDGE = std::numeric_limits<E>::max();

    explicit AdjMtxGraph(bool directed = false) : directed{directed} {}

    // ════════════════════════════════════════════
    //  Wierzchołki
    // ════════════════════════════════════════════

    // O(V) — rozszerzenie macierzy o nowy wiersz i kolumnę
    VertexPtr addVertex(V newVertex) override {
        VertexID id  = nextVID++;
        size_t   idx = reverseMap.size();

        indexMap[id] = idx;
        reverseMap.push_back(id);

        auto vertex = std::make_shared<Vertex<V,E>>(id, std::move(newVertex));
        vertexMap[id] = vertex;

        // Rozszerz macierz wag i macierz wskaźników o nową kolumnę
        for (auto& row : matrix)   row.push_back(NO_EDGE);
        for (auto& row : edgePtrs) row.push_back(nullptr);

        // Dodaj nowy wiersz
        matrix.push_back(std::vector<E>(idx + 1, NO_EDGE));
        edgePtrs.push_back(std::vector<EdgePtr>(idx + 1, nullptr));

        return vertex;
    }

    // O(V) — wyzerowanie wiersza i kolumny
    void eraseVertex(VertexID) override;

    VertexPtr getVertex(VertexID vID) const override {
        auto it = vertexMap.find(vID);
        if (it == vertexMap.end()) throw VertexNotFoundException(vID);
        return it->second;
    }

    bool hasVertex(VertexID vID) const override {
        return vertexMap.contains(vID);
    }

    std::vector<VertexPtr> vertices() const override {
        std::vector<VertexPtr> result;
        for (const auto& [_, v] : vertexMap) result.push_back(v);
        return result;
    }

    size_t vertexCount() const override { return vertexMap.size(); }

    // ════════════════════════════════════════════
    //  Krawędzie
    // ════════════════════════════════════════════

    // O(1) — bezpośredni zapis do macierzy
    EdgePtr addEdge(VertexID fromID, VertexID toID, E weight) override {
        if (hasEdge(fromID, toID)) throw DuplicateEdgeException(fromID, toID);

        size_t r = indexMap.at(fromID);
        size_t c = indexMap.at(toID);

        matrix[r][c] = weight;
        if (!directed) matrix[c][r] = weight;

        EdgeID eid = nextEID++;
        EdgePtr edge;
        if (directed)
            edge = std::make_shared<DirectedEdge<E>>(eid, fromID, toID, weight);
        else
            edge = std::make_shared<Edge<E>>(eid, fromID, toID, weight);

        // Store edge pointer in edgePtrs matrix
        edgePtrs[r][c] = edge;
        if (!directed) edgePtrs[c][r] = edge;

        ++edgeCount_;

        return edge;
    }

    // O(1) — bezpośrednie wyzerowanie komórek macierzy
    void eraseEdge(VertexID fromID, VertexID toID) override {
        if (!hasEdge(fromID, toID)) throw EdgeNotFoundException(fromID, toID);

        size_t r = indexMap.at(fromID);
        size_t c = indexMap.at(toID);

        // Remove edge from matrix and edgePtrs
        matrix[r][c] = NO_EDGE;
        if (!directed) matrix[c][r] = NO_EDGE;

        // Remove edge pointer(s)
        if (edgePtrs[r][c]) {
            edgePtrs[r][c] = nullptr;
            --edgeCount_;
        }
        if (!directed && edgePtrs[c][r]) {
            edgePtrs[c][r] = nullptr;
        }
    }

    // O(1) — jedno sprawdzenie macierzy
    bool hasEdge(VertexID from, VertexID to) const override {
        auto it_i = indexMap.find(from);
        auto it_j = indexMap.find(to);
        if (it_i == indexMap.end() || it_j == indexMap.end()) return false;
        return matrix[it_i->second][it_j->second] != NO_EDGE;
    }

    // O(1) — odczyt wskaźnika z edgePtrs
    std::optional<EdgePtr> findEdge(VertexID from, VertexID to) const override {
        auto it_i = indexMap.find(from);
        auto it_j = indexMap.find(to);
        if (it_i == indexMap.end() || it_j == indexMap.end())
            return std::nullopt;
        const auto& ep = edgePtrs[it_i->second][it_j->second];
        if (!ep) return std::nullopt;
        return ep;
    }

    // O(V²) — jedyna operacja wymagająca przejścia całej macierzy
    std::vector<EdgePtr> edges() const override {
        std::vector<EdgePtr> result;
        result.reserve(edgeCount_);
        const size_t n = reverseMap.size();
        // Iterujemy tylko górny trójkąt (i < j) — każda krawędź raz
        for (size_t i = 0; i < n; ++i) {
            if (!vertexMap.contains(reverseMap[i])) continue;
            for (size_t j = i + 1; j < n; ++j) {
                if (!vertexMap.contains(reverseMap[j])) continue;
                if (edgePtrs[i][j]) result.push_back(edgePtrs[i][j]);
            }
        }
        return result;
    }

    // O(1) — utrzymywany licznik
    size_t edgeCount() const override { return edgeCount_; }

    // ════════════════════════════════════════════
    //  Sąsiedztwo i stopień
    // ════════════════════════════════════════════

    // O(V) — skan wiersza macierzy
    std::vector<VertexPtr> neighbors(VertexID vID) const override {
        std::vector<VertexPtr> result;
        size_t r = indexMap.at(vID);
        for (size_t c = 0; c < reverseMap.size(); ++c) {
            if (matrix[r][c] != NO_EDGE && vertexMap.contains(reverseMap[c]))
                result.push_back(vertexMap.at(reverseMap[c]));
        }
        return result;
    }

    // O(V) — zliczanie niezerowych komórek w wierszu
    size_t degree(VertexID vID) const override {
        size_t r = indexMap.at(vID);
        size_t count = 0;
        for (size_t c = 0; c < reverseMap.size(); ++c)
            if (matrix[r][c] != NO_EDGE) ++count;
        return count;
    }

    // O(V²) — iteracja całej macierzy (symetryczna → oba kierunki gratis)
    void forEachEdge(const typename Graph<V,E>::EdgeVisitor& fn) const override {
        const size_t n = reverseMap.size();
        for (size_t i = 0; i < n; ++i) {
            if (!vertexMap.contains(reverseMap[i])) continue;
            for (size_t j = 0; j < n; ++j) {
                if (!vertexMap.contains(reverseMap[j])) continue;
                if (matrix[i][j] != NO_EDGE)
                    fn(reverseMap[i], reverseMap[j], matrix[i][j]);
            }
        }
    }

    void print() const override {
        const size_t n = reverseMap.size();
        std::cout << "     ";
        for (size_t c = 0; c < n; ++c)
            if (vertexMap.contains(reverseMap[c]))
                std::cout << this->getVertex(reverseMap[c])->getName() << "  ";
        std::cout << "\n";

        for (size_t r = 0; r < n; ++r) {
            if (!vertexMap.contains(reverseMap[r])) continue;
            std::cout << this->getVertex(reverseMap[r])->getName() << ":  ";
            for (size_t c = 0; c < n; ++c)
                std::cout << (matrix[r][c] == NO_EDGE ? 0 : matrix[r][c]) << "  ";
            std::cout << "\n";
        }
    }

private:
    // Macierz wag — główna struktura danych
    std::vector<std::vector<E>>       matrix;    // O(V²) pamięci

    // Macierz wskaźników równoległa do matrix
    // edgePtrs[i][j] == edgePtrs[j][i] dla krawędzi nieskierowanej
    // Eliminuje potrzebę edgeMap przy zachowaniu O(1) findEdge
    std::vector<std::vector<EdgePtr>> edgePtrs;  // O(V²) pamięci

    // Zarządzanie wierzchołkami
    std::unordered_map<VertexID, VertexPtr> vertexMap;
    std::unordered_map<VertexID, size_t>    indexMap;   // VertexID → indeks macierzy
    std::vector<VertexID>                   reverseMap; // indeks → VertexID

    bool directed; // flaga dla grafu skierowanego/nieskierowanego
    size_t   edgeCount_ = 0;  // licznik utrzymywany przy add/erase
    VertexID nextVID    = 0;
    EdgeID   nextEID    = 0;
};

// ─────────────────────────────────────────────────────────────

// O(V) — wyzerowanie wiersza i kolumny + aktualizacja licznika
template <typename V, typename E>
void AdjMtxGraph<V,E>::eraseVertex(VertexID old) {
    size_t idx = indexMap.at(old);
    const size_t n = reverseMap.size();

    // Policz usuwane krawędzie (tylko aktywne wierzchołki)
    for (size_t c = 0; c < n; ++c) {
        if (matrix[idx][c] != NO_EDGE && vertexMap.contains(reverseMap[c])) {
            matrix[idx][c] = matrix[c][idx] = NO_EDGE;
            edgePtrs[idx][c] = edgePtrs[c][idx] = nullptr;
            --edgeCount_;
        }
    }

    indexMap.erase(old);
    vertexMap.erase(old);
}