#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "Edge.h"
#include "Utilities.h"

/**
 * class Vertex
 * represents a vertex in a graph
 * @tparam V the type of name of the vertex, @tparam E the type of weight of the edge
 */
template <typename V, typename E>
class Vertex {
public:

    /**
     * public type for a pointer pointing to an object of @class Edge
     */
    using EdgePtr = std::shared_ptr<Edge<E>>;
    /**
     * public type for a list of edge pointers incident to this vertex. only used in @class AdjListGraph
     */
    using EdgeList = std::vector<EdgePtr>;

    Vertex();
    Vertex(VertexID, V);
    ~Vertex();

    /**
     * public method for adding a new edge to the vertex's incident edges list
     * @param newEdge the edge to be added
     * uses std::move to move the edge pointer to the edges list of the vertex and avoid unnecessary copying of the edge object
     */
    void addEdge(EdgePtr newEdge) {edges.push_back(std::move(newEdge));}

    void removeEdge(EdgeID);

    const EdgeList& incidentEdges() const {return edges;}
    bool isAdjacentTo(const Vertex<V,E>& other) const;

    EdgeList outEdges() const {
    EdgeList result;
    for (const auto& e : edges) {
        // krawędź skierowana: tylko wychodząca
        // krawędź nieskierowana: każda incydentna jest "wychodząca"
        if (!e->isDirected() || e->getFrom() == id)
            result.push_back(e);
    }
    return result;
}
    EdgeList inEdges() const {
        EdgeList result;
        for (const auto& e : edges)
            if (e->isDirected() && e->getTo() == id)
                result.push_back(e);
        return result;
    }

    // getters and setters for vertex attributes

    V getName() const {return name;}
    void setName(V newName) {name = newName;}

    VertexID getID() const {return id;}
    void setID(VertexID newID) {id = newID;}

    /**
     * public method for returning the degree of vertex
     * @return degree of vertex
     */
    size_t degree() const {return edges.size();}
    size_t outDegree() const {return outEdges().size();}
    size_t inDegree() const {return inEdges().size();}  

private:
    VertexID    id;
    V           name;
    EdgeList    edges;
};

/**
 * non-parameter constructor of @class Vertex
 * @tparam V type of name of the vertex, @tparam E type of weight of the edge
 * sets all vertex attributes to 0 or empty
 */
template <typename V, typename E>
Vertex<V,E>::Vertex() : id{0}, name{} {}

/**
 * parameter constructor of @class Vertex
 * @tparam V type of name of the vertex, @tparam E type of weight of the edge
 * @param newID the ID of the vertex, @param newName the name of the vertex
 * sets the vertex attributes to the given parameters
 */
template <typename V, typename E>
Vertex<V,E>::Vertex(VertexID newID, V newName) : id{newID}, name{newName} {}

/**
 * destructor of @class Vertex
 */
template <typename V, typename E>
Vertex<V,E>::~Vertex() {}

/**
 * public method for removing an edge from the vertex's incident edges list
 * @tparam V type of name of the vertex, @tparam E type of weight of the edge
 * @param old the ID of edge to be removed
 * uses erase to remove the edge and remove_if to find the pointer to the edge with the given ID.
 */
template<typename V, typename E>
void Vertex<V,E>::removeEdge(EdgeID old) {
    edges.erase(
            std::remove_if(edges.begin(), edges.end(),
                [old](const EdgePtr& e){ return e->getID() == old; }),
            edges.end());
}

/**
 * public method for checking if the vertex is adjacent to another vertex
 * @param other the other vertex to be checked for adjacency
 * @return true if the vertex is adjacent to the other vertex, otherwise false
 * iterates through the incident edges of this vertex and checks with @fn opposite() if the next vertex is opposite to the source
 */
template <typename V, typename E>
bool Vertex<V,E>::isAdjacentTo(const Vertex<V,E>& other) const {
    for (const auto& edge : incidentEdges()) {
        if (edge->opposite(id) == other.getID()) {
            return true;
        }
    }
    return false;
}

