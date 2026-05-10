#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "Edge.h"
#include "Utilities.h"

template <typename V, typename E>
class Vertex {
public:
    using EdgePtr = std::shared_ptr<Edge<E>>;
    using EdgeList = std::vector<EdgePtr>;

    Vertex();
    Vertex(VertexID, V);
    ~Vertex();

    void addEdge(EdgePtr newEdge) {edges.push_back(std::move(newEdge));}

    void removeEdge(EdgeID);

    const EdgeList& incidentEdges() const {return edges;}
    bool isAdjacentTo(const Vertex<V,E>& other) const;

    EdgeList outEdges() const {
        EdgeList result;
        for (const auto& e : edges) {
            if(e->getFrom() == id) {result.push_back(e);}
        }
        return result;
    }
    EdgeList inEdges() const {
        EdgeList result;
        for (const auto& e : edges) {
            if(e->getTo() == id) {result.push_back(e);}
        }
        return result;
    }

    V getName() const {return name;}
    void setName(V newName) {name = newName;}

    VertexID getID() const {return id;}
    void setID(VertexID newID) {id = newID;}

    size_t degree() const {return edges.size();}
    size_t outDegree() const {return outEdges().size();}
    size_t inDegree() const {return inEdges().size();}  

private:
    VertexID    id;
    V           name;
    EdgeList    edges;
};

template <typename V, typename E>
Vertex<V,E>::Vertex() : id{0}, name{} {}

template <typename V, typename E>
Vertex<V,E>::Vertex(VertexID newID, V newName) : id{newID}, name{newName} {}

template <typename V, typename E>
Vertex<V,E>::~Vertex() {}

template<typename V, typename E>
void Vertex<V,E>::removeEdge(EdgeID old) {
    edges.erase(
            std::remove_if(edges.begin(), edges.end(),
                [old](const EdgePtr& e){ return e->getID() == old; }),
            edges.end());
}
template <typename V, typename E>
bool Vertex<V,E>::isAdjacentTo(const Vertex<V,E>& other) const {
    for (const auto& edge : incidentEdges()) {
        if (edge->opposite(id) == other.getID()) {
            return true;
        }
    }
    return false;
}

