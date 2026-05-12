#pragma once
#include "Graph.h"
#include <vector>
#include <unordered_map>
#include <limits>
#include <iostream>

/**
 * class AdjMtxGraph
 * inherits from @class Graph and implements a graph data structure using an adjacency matrix representation
 * @tparam V type of name of the vertex, @tparam E type of weight
 * the acjadency matrix is a vector of vectors of weights
 * uses unordered maps to store mapping ID's to pointers of vertex objects as values and ID's as keys to allow for O(1) access to the vertex objects.
 * uses two counters for keeping track of the next available ID for vertex and edge objects
 */
template <typename V, typename E>
class AdjMtxGraph : public Graph<V,E> {
public:
    using VertexPtr = std::shared_ptr<Vertex<V,E>>;
    using EdgePtr   = std::shared_ptr<Edge<E>>;

    static constexpr E NO_EDGE = std::numeric_limits<E>::max();

    explicit AdjMtxGraph(bool directed = false) : directed{directed} {}

    // vertex functions

    /**
     * public method for adding a new vertex to the graph
     * @param newVertex name of the new vertex to be added
     * @return pointer to the newly added vertex object
     * adds the vertex to the graph, and adds the vertex ID and pointer as pair to the map to allow for O(1) access
     */
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

    void eraseVertex(VertexID) override;

    /**
     * public method for returning a pointer to the vertex object with the given ID
     * @param vID ID of vertex to be returned
     * @return pointer to the vertex object with the given ID if available, otherwise @throw VertexNotFoundException
     * uses the vertex map to find the vertex, instead of iterating through the list of vertices
     */
    VertexPtr getVertex(VertexID vID) const override {
        auto it = vertexMap.find(vID);
        if (it == vertexMap.end()) throw VertexNotFoundException(vID);
        return it->second;
    }

    bool hasVertex(VertexID vID) const override {
        return vertexMap.contains(vID);
    }

    /**
     * public method for returning a vector of pointers to all vertices in the graph
     * @return vector of pointers to all vertices
     * iterates through the vertex map, ignores the keys and adds the vertex pointers to the result
     */
    std::vector<VertexPtr> vertices() const override {
        std::vector<VertexPtr> result;
        for (const auto& [_, v] : vertexMap) result.push_back(v);
        return result;
    }

    size_t vertexCount() const override { return vertexMap.size(); }

    // edge functions

    /**
     * public method for adding a new edge to the graph
     * @param from source vertex ID, @param to destination vertex ID, @param weight weight of the edge
     * @return pointer to the newly added edge object
     * checks if edge already exists; if so, @throws DuplicateEdgeException. Otherwise, proceeds
     * gets the vertex object from map using keys passed as parameters
     * creates a new edge and adds it to the matrix and edgePtrs matrix, instead of iterating through the list of edges
     */
    EdgePtr addEdge(VertexID fromID, VertexID toID, E weight) override {
        if (hasEdge(fromID, toID)) {throw DuplicateEdgeException(fromID, toID);}

        size_t r = indexMap.at(fromID);
        size_t c = indexMap.at(toID);

        matrix[r][c] = weight;
        if (!directed) {matrix[c][r] = weight;}

        EdgeID eid = nextEID++;
        EdgePtr edge;
        if (directed) { edge = std::make_shared<DirectedEdge<E>>(eid, fromID, toID, weight);}
        else {edge = std::make_shared<Edge<E>>(eid, fromID, toID, weight);}

        edgePtrs[r][c] = edge;
        if (!directed) {edgePtrs[c][r] = edge;}

        ++edgeCount_;

        return edge;
    }

    /**
     * public method for erasing an edge from the graph
     * @tparam V type of name of the vertex, @tparam E type of weight
     * @param from source vertex ID, @param to destination vertex ID
     * @throws EdgeNotFoundException if there is no edge between the given vertices
     * gets the edge object from map using keys passed as parameters
     */
    void eraseEdge(VertexID fromID, VertexID toID) override {
        if (!hasEdge(fromID, toID)) {throw EdgeNotFoundException(fromID, toID);}

        size_t r = indexMap.at(fromID);
        size_t c = indexMap.at(toID);

        // Remove edge from matrix and edgePtrs
        matrix[r][c] = NO_EDGE;
        if (!directed) {matrix[c][r] = NO_EDGE;}

        // Remove edge pointer(s)
        if (edgePtrs[r][c]) { edgePtrs[r][c] = nullptr; --edgeCount_;}
        if (!directed && edgePtrs[c][r]) { edgePtrs[c][r] = nullptr;}
    }

    /**
     * public method for checking if there is an edge between two vertices
     * @param fromID source vertex ID, @param toID destination vertex ID
     * @return true if there is an edge between the given vertices, otherwise false
     * uses the matrix to check if there is an edge, instead of iterating through the
     */
    bool hasEdge(VertexID from, VertexID to) const override {
        auto it_i = indexMap.find(from);
        auto it_j = indexMap.find(to);
        if (it_i == indexMap.end() || it_j == indexMap.end()) return false;
        return matrix[it_i->second][it_j->second] != NO_EDGE;
    }

    /**
     * public method for finding an edge in the graph
     * @param fromID source vertex ID, @param toID destination vertex ID
     * @return pointer to the edge object if available, otherwise nullopt
     * uses the edgePtrs matrix to find the edge, instead of iterating through the list of edges
     */
    std::optional<EdgePtr> findEdge(VertexID from, VertexID to) const override {
        auto it_i = indexMap.find(from);
        auto it_j = indexMap.find(to);
        if (it_i == indexMap.end() || it_j == indexMap.end())
            return std::nullopt;
        const auto& ep = edgePtrs[it_i->second][it_j->second];
        if (!ep) return std::nullopt;
        return ep;
    }

    /**
     * public method for returning a vector of pointers to all edges in the graph
     * @return vector of pointers to all edges
     * iterates through the edgePtrs matrix and adds non-nullptr edge pointers to the result;
     */
    std::vector<EdgePtr> edges() const override {
        std::vector<EdgePtr> result;
        result.reserve(edgeCount_);
        const size_t n = reverseMap.size();
        if (directed) {
            // Graf skierowany: pełna macierz — każda komórka to osobna krawędź
            for (size_t i = 0; i < n; ++i) {
                if (!vertexMap.contains(reverseMap[i])) {continue;}
                for (size_t j = 0; j < n; ++j) {
                    if (!vertexMap.contains(reverseMap[j])) {continue;}
                    if (edgePtrs[i][j]) {result.push_back(edgePtrs[i][j]);}
                }
            }
        } else {
            // Graf nieskierowany: tylko górny trójkąt — każda krawędź raz
            for (size_t i = 0; i < n; ++i) {
                if (!vertexMap.contains(reverseMap[i])) {continue;}
                for (size_t j = i + 1; j < n; ++j) {
                    if (!vertexMap.contains(reverseMap[j])) {continue;}
                    if (edgePtrs[i][j]) {result.push_back(edgePtrs[i][j]);}
                }
            }
        }
        return result;
    }

    size_t edgeCount() const override { return edgeCount_; }

    // utilities

    std::vector<VertexPtr> neighbors(VertexID vID) const override {
        std::vector<VertexPtr> result;
        size_t r = indexMap.at(vID);
        for (size_t c = 0; c < reverseMap.size(); ++c) {
            if (matrix[r][c] != NO_EDGE && vertexMap.contains(reverseMap[c])) { result.push_back(vertexMap.at(reverseMap[c]));}
        }
        return result;
    }

    size_t degree(VertexID vID) const override {
        size_t r = indexMap.at(vID);
        size_t count = 0;
        for (size_t c = 0; c < reverseMap.size(); ++c)
            if (matrix[r][c] != NO_EDGE) ++count;
        return count;
    }

    void forEachEdge(const typename Graph<V,E>::EdgeVisitor& fn) const override {
        const size_t n = reverseMap.size();
        for (size_t i = 0; i < n; ++i) {
            if (!vertexMap.contains(reverseMap[i])) { continue;}
            for (size_t j = 0; j < n; ++j) {
                if (!vertexMap.contains(reverseMap[j])) { continue;}
                if (matrix[i][j] != NO_EDGE) { fn(reverseMap[i], reverseMap[j], matrix[i][j]);}
            }
        }
    }

    void print() const override {
        const size_t n = reverseMap.size();
        std::cout << "     ";
        for (size_t c = 0; c < n; ++c)
            if (vertexMap.contains(reverseMap[c])) { std::cout << this->getVertex(reverseMap[c])->getName() << "  ";}
        std::cout << "\n";

        for (size_t r = 0; r < n; ++r) {
            if (!vertexMap.contains(reverseMap[r])) { continue;}
            std::cout << this->getVertex(reverseMap[r])->getName() << ":  ";
            for (size_t c = 0; c < n; ++c) { std::cout << (matrix[r][c] == NO_EDGE ? 0 : matrix[r][c]) << "  ";}
            std::cout << "\n";
        }
    }

    bool isDirected() const override { return directed; }

private:
    std::vector<std::vector<E>>       matrix;
    std::vector<std::vector<EdgePtr>> edgePtrs;  

    std::unordered_map<VertexID, VertexPtr> vertexMap;
    std::unordered_map<VertexID, size_t>    indexMap;   // VertexID → matrix index
    std::vector<VertexID>                   reverseMap; // index → VertexID

    bool directed;
    size_t   edgeCount_ = 0;
    VertexID nextVID    = 0;
    EdgeID   nextEID    = 0;
};

/**
 * public method for erasing a vertex from the graph
 * @tparam V type of name of the vertex, @tparam E type of weight of the edge
 * @param old ID of vertex to be erased
 * gets the vertex object from map using key passed as parameter
 * iterates through the vector of edges to be removed and calls @fn eraseEdgeByID() to remove the edge from the graph
 * erases the vertex from the vertex map
 */
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