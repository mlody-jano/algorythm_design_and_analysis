#pragma once
#include "Graph.h"
#include <unordered_map>
#include <iostream>

template <typename V, typename E>
class AdjListGraph : public Graph<V,E> {
public:
    using VertexPtr = std::shared_ptr<Vertex<V,E>>;
    using EdgePtr   = std::shared_ptr<Edge<E>>;

    // vertex functions

    VertexPtr addVertex(V newVertex) override {
        VertexID id = nextVID++;

        auto vertex = std::make_shared<Vertex<V,E>>(id, std::move(newVertex));
        vertexMap[id] = vertex;
        return vertex;
    }

    void eraseVertex(VertexID) override;

    VertexPtr getVertex(VertexID vID) const override {
        auto it = vertexMap.find(vID);
        if(it == vertexMap.end()) {throw VertexNotFoundException(vID);}
        return it->second;
    }

    bool hasVertex(VertexID vID) const override { return vertexMap.contains(vID);}

    std::vector<VertexPtr> vertices() const override {
        std::vector<VertexPtr> result;
        for (const auto& [_, vertex] : vertexMap) { result.push_back(vertex);}
        return result;
    }

    size_t vertexCount() const override { return vertexMap.size();}

    // edge functions

    EdgePtr addEdge(VertexID from, VertexID to, E weight) override {
        if(this->hasEdge(from,to)) {throw DuplicateEdgeException(from,to);}

        auto source = this->getVertex(from);
        auto dest = this->getVertex(to);

        EdgeID eID = nextEID++;
        EdgePtr newEdge;

        newEdge = std::make_shared<Edge<E>>(eID, from, to, weight);

        edgeMap[eID] = newEdge;

        source->addEdge(newEdge);
        dest->addEdge(newEdge);

        return newEdge; 
    }

    void eraseEdge(VertexID, VertexID) override;

    bool hasEdge(VertexID fromID, VertexID toID) const override { return this->findEdge(fromID, toID).has_value();}

    std::optional<EdgePtr> findEdge(VertexID fromID, VertexID toID) const override {
        if (!this->hasVertex(fromID)) return std::nullopt;
        for (const auto& edge : this->getVertex(fromID)->incidentEdges()) {
            if (edge->getFrom() == fromID && edge->getTo() == toID) {return edge;}
            if (edge->getFrom() == toID && edge->getTo() == fromID) {return edge;}
        }
        return std::nullopt;
    }

    std::vector<EdgePtr> edges() const override {
        std::vector<EdgePtr> result;
        for (const auto& [_, edge] : edgeMap) result.push_back(edge);
        return result;
    }

    size_t edgeCount() const override { return edgeMap.size(); }

    // utilities

    
    std::vector<VertexPtr> neighbors(VertexID vID) const override {
        std::vector<VertexPtr> result;
        for (const auto& edge : this->getVertex(vID)->incidentEdges())
            if (auto neigh = edge->opposite(vID)) {result.push_back(this->getVertex(*neigh));}
        return result;
    }

    void print() const override;

    size_t degree(VertexID) const override;

private:
    std::unordered_map<VertexID, VertexPtr> vertexMap;
    std::unordered_map<EdgeID,   EdgePtr>   edgeMap;
    VertexID nextVID = 0;
    EdgeID   nextEID = 0;

    void eraseEdgeByID(EdgeID eID) {
        auto it = edgeMap.find(eID);

        if(it == edgeMap.end()) { return;}
        auto& edge = it->second;

        if(this->hasVertex(edge->getFrom())) { this->getVertex(edge->getFrom())->removeEdge(eID);}
        if(this->hasVertex(edge->getTo()))  { this->getVertex(edge->getTo())->removeEdge(eID);}

        edgeMap.erase(eID);
    }
};

template <typename V, typename E>
void AdjListGraph<V,E>::eraseVertex(VertexID old) {
    auto vertex = this->getVertex(old);

    std::vector<EdgeID> toBeRemoved;

    for (const auto& edge : vertex->incidentEdges()) { toBeRemoved.push_back(edge->getID());}
    for (EdgeID eID : toBeRemoved) { eraseEdgeByID(eID);}
    vertexMap.erase(old);
}

template <typename V, typename E>
void AdjListGraph<V,E>::eraseEdge(VertexID from, VertexID to) {
    auto edge = this->findEdge(from, to);
    if (!edge) {throw EdgeNotFoundException(from, to);}
    eraseEdgeByID((*edge)->getID());
}

template <typename V, typename E>
void AdjListGraph<V,E>::print() const {
    for (const auto& [_, vertex] : vertexMap) {
            std::cout << vertex->getName() << ": ";
            for (const auto& neigh : neighbors(vertex->getID()))
                std::cout << neigh->getName() << " ";
            std::cout << "\n";
    }
}

template <typename V, typename E>
size_t AdjListGraph<V,E>::degree(VertexID vID) const { return this->getVertex(vID)->degree();}