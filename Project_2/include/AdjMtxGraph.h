#pragma once
#include "Graph.h"
#include "Vertex.h"
#include "Edge.h"
#include <vector>
#include <unordered_map>
#include <limits>
#include <iostream>

template <typename V, typename E>
class AdjMtxGraph : public Graph<V,E> {
public:

    using VertexPtr = std::shared_ptr<Vertex<V,E>>;
    using EdgePtr = std::shared_ptr<Edge<E>>;

    static constexpr E NO_EDGE = std::numeric_limits<E>::max();

    // vertex functions

    VertexPtr addVertex(V newVertex) override {
        VertexID id = nextVID++;

        // Mapping VertexID to position in adjacency matrix
        size_t idx = indexMap.size();
        indexMap[id] = idx;
        reverseMap.push_back(id);

        auto vertex = std::make_shared<Vertex<V,E>>(id, std::move(newVertex));
        vertexMap[id] = vertex;

        for(auto& row : matrix) {row.push_back(NO_EDGE);}
        matrix.push_back(std::vector<E>(idx + 1, NO_EDGE));

        return vertex;

    }
    void eraseVertex(VertexID) override;

    VertexPtr getVertex(VertexID vID) const override {
        auto it = vertexMap.find(vID);
        if(it == vertexMap.end()) {throw VertexNotFoundException(vID);}
        return it->second;
    }

    bool hasVertex(VertexID vID) const override {return vertexMap.contains(vID);}

    std::vector<VertexPtr> vertices() const override {
        std::vector<VertexPtr> result;
        for(const auto& [_,vertex]: vertexMap) {result.push_back(vertex);}
        return result;
    }

    size_t vertexCount() const override {return vertexMap.size();}

    // edge functions

    EdgePtr addEdge(VertexID from, VertexID to, E weight) override {
        if(this->hasEdge(from,to)) {throw DuplicateEdgeException(from,to);}

        size_t source = indexMap.at(from);
        size_t dest = indexMap.at(to);

        matrix[source][dest] = weight;
        matrix[dest][source] = weight;

        EdgeID eID = nextEID++;
        EdgePtr newEdge;

        newEdge = std::make_shared<Edge<E>>(eID, from, to, weight);

        edgeMap[eID] = newEdge;

        return newEdge;
    }

    void eraseEdge(VertexID, VertexID) override;
    bool hasEdge(VertexID, VertexID) const override;

    std::optional<EdgePtr> findEdge(VertexID from, VertexID to) const override {
        for(const auto& [_, edge] : edgeMap) {
            if(edge->getFrom() == from && edge->getTo() == to) {return edge;}
        }
        return std::nullopt;
    }

    std::vector<EdgePtr> edges() const override {
        std::vector<EdgePtr> result;
        for(const auto& [_, edge] : edgeMap) {result.push_back(edge);}
        return result;
    }

    size_t edgeCount() const override {return edgeMap.size();}
    
    // utilities

    std::vector<VertexPtr> neighbors(VertexID vID) const override {
        std::vector<VertexPtr> result;
        size_t r = indexMap.at(vID);
        for (size_t c = 0; c < matrix[r].size(); ++c) {
            if (matrix[r][c] != NO_EDGE) {
                VertexID neighID = reverseMap[c];
                if (vertexMap.contains(neighID))
                    result.push_back(vertexMap.at(neighID));
            }
        }
        return result;
    }

    void print() const override {
        std::cout << "   ";
        for (VertexID vID : reverseMap)
            if (vertexMap.contains(vID))
                std::cout << this->getVertex(vID)->getName() << " ";
        std::cout << "\n";

        for (size_t r = 0; r < matrix.size(); ++r) {
            VertexID vID = reverseMap[r];
            if (!vertexMap.contains(vID)) continue;
            std::cout << this->getVertex(vID)->getName() << ": ";
            for (const auto& w : matrix[r])
                std::cout << (w == NO_EDGE ? 0 : w) << " ";
            std::cout << "\n";
        }
    }

    size_t degree(VertexID) const override;

private:
    std::unordered_map<VertexID, VertexPtr> vertexMap;  // mapping vertexID to actual vertex object pointers
    std::unordered_map<EdgeID,   EdgePtr>   edgeMap;    // mapping edgeID to actual edge object pointers
    std::unordered_map<VertexID, size_t>    indexMap;   // mapping vertexID to position in adjacency matrix
    std::vector<VertexID>                   reverseMap; // reversal of mapping from above (position in adj matrix -> vertexID)
    std::vector<std::vector<E>>             matrix;     // adjacency matrix
    VertexID nextVID = 0;
    EdgeID   nextEID = 0;

};

template <typename V, typename E>
void AdjMtxGraph<V,E>::eraseVertex(VertexID old) {
        size_t idx = indexMap.at(old);

        for(auto& row : matrix) {row[idx] = NO_EDGE;}
        for(auto& value : matrix[idx]) {value = NO_EDGE;}

        std::vector<EdgeID> toBeRemoved;
        for(const auto& [eID,e] : edgeMap) {
            if(e->isIncidentOn(old)) {toBeRemoved.push_back(eID);}
        }
        for (EdgeID eID : toBeRemoved) {edgeMap.erase(eID);}

        indexMap.erase(old);
        vertexMap.erase(old);
}

template <typename V, typename E>
void AdjMtxGraph<V,E>::eraseEdge(VertexID from, VertexID to) {
    if(!this->hasEdge(from, to)) {throw EdgeNotFoundException(from, to);}

    size_t source = indexMap.at(from);
    size_t dest = indexMap.at(to);

    matrix[source][dest] = NO_EDGE;
    matrix[dest][source] = NO_EDGE;

    // Usuń z mapy krawędzi
    for (auto it = edgeMap.begin(); it != edgeMap.end(); ++it) {
            auto& e = it->second;
            if (e->getFrom() == from && e->getTo() == to) {
                edgeMap.erase(it);
                break;
            }
    }
}

template <typename V, typename E>
bool AdjMtxGraph<V,E>::hasEdge(VertexID from, VertexID to) const {
    auto source = indexMap.find(from);
    auto dest = indexMap.find(to);

    if(source == indexMap.end() || dest == indexMap.end()) {return false;}
    return matrix[source->second][dest->second] != NO_EDGE;
}

template <typename V, typename E>
size_t AdjMtxGraph<V,E>::degree(VertexID vID) const {
    size_t r = indexMap.at(vID);
    size_t count = 0;
    for (size_t c = 0; c < matrix[r].size(); ++c)
        if (matrix[r][c] != NO_EDGE) ++count;
    return count;
}