#pragma once
#include "Graph.h"
#include <unordered_map>
#include <iostream>

/**
 * class AdjListGraph
 * inherits from @class Graph and implements a graph data structure using an adjacency list representation
 * @tparam V type of name of the vertex, @tparam E type of weight of the edge
 * the adjacency list itself is implemented by the vector of edge pointers in the @class Vertex
 * uses unordered maps to store mapping ID's to pointers of vertex and edge objects as values and ID's as keys to allow for O(1) access to the vertex and edge objects.
 * uses two counters for keeping track of the next available ID for vertex and edge objects
 */
template <typename V, typename E>
class AdjListGraph : public Graph<V,E> {
public:
    using VertexPtr = std::shared_ptr<Vertex<V,E>>;
    using EdgePtr   = std::shared_ptr<Edge<E>>;

    // vertex functions

    explicit AdjListGraph(bool directed = false) : directed(directed) {}

    /**
     * public method for adding a new vertex to the graph
     * @param newVertex name of the new vertex to be added
     * @return pointer to the newly added vertex object
     * adds the vertex to the graph, and adds the vertex ID and pointer as pair to the map to allow for O(1) access
     */
    VertexPtr addVertex(V newVertex) override {
        VertexID id = nextVID++;

        auto vertex = std::make_shared<Vertex<V,E>>(id, std::move(newVertex));
        vertexMap[id] = vertex;
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
        if(it == vertexMap.end()) {throw VertexNotFoundException(vID);}
        return it->second;
    }

    bool hasVertex(VertexID vID) const override { return vertexMap.contains(vID);}

    /**
     * public method for returning a vector of pointers to all vertices in the graph
     * @return vector of pointers to all vertices
     * iterates through the vertex map, ignores the keys and adds the vertex pointers to the result
     */
    std::vector<VertexPtr> vertices() const override {
        std::vector<VertexPtr> result;
        for (const auto& [_, vertex] : vertexMap) { result.push_back(vertex);}
        return result;
    }

    size_t vertexCount() const override { return vertexMap.size();}

    // edge functions

    /**
     * public method for adding a new edge to the graph
     * @param from source vertex ID, @param to destination vertex ID, @param weight weight of the edge
     * @return pointer to the newly added edge object
     * checks if edge already exists; if so, @throws DuplicateEdgeException. Otherwise, proceeds
     * gets the vertex object from map using keys passed as parameters, instead of iterating through the list of vertices
     * creates a new edge and adds it to the edge map
     */
    EdgePtr addEdge(VertexID fromID, VertexID toID, E weight) override {
        if (hasEdge(fromID, toID)) throw DuplicateEdgeException(fromID, toID);

        auto from = getVertex(fromID);
        auto to   = getVertex(toID);

        EdgeID eid = nextEID++;
        EdgePtr edge;
        if (directed)
            edge = std::make_shared<DirectedEdge<E>>(eid, fromID, toID, weight);
        else
            edge = std::make_shared<Edge<E>>(eid, fromID, toID, weight);

        edgeMap[eid] = edge;

        // Oba wierzchołki wiedzą o krawędzi
        from->addEdge(edge);
        to->addEdge(edge);

        return edge;
    }

    void eraseEdge(VertexID, VertexID) override;

    bool hasEdge(VertexID fromID, VertexID toID) const override { return this->findEdge(fromID, toID).has_value();}

    /**
     * public method for finding an edge in the graph
     * @param fromID source vertex ID, @param toID destination vertex ID
     * @return pointer to the edge object if available, otherwise nullopt
     * iterates through incident edges of the source vertex and checks if there is a matching edge with the destination vertex
     */
    std::optional<EdgePtr> findEdge(VertexID fromID,
                                    VertexID toID) const override {
        if (!hasVertex(fromID)) return std::nullopt;
        for (const auto& e : getVertex(fromID)->incidentEdges()) {
            if (e->getFrom() == fromID && e->getTo() == toID) return e;
            if (!directed && e->getFrom() == toID && e->getTo() == fromID)
                return e;
        }
        return std::nullopt;
    }

    /**
     * public method for returning a vector of pointers to all edges in the graph
     * @return vector of pointers to all edges
     * iterates through the edge map, ignores the keys and adds the edge pointers to the result
     */
    std::vector<EdgePtr> edges() const override {
        std::vector<EdgePtr> result;
        for (const auto& [_, edge] : edgeMap) {result.push_back(edge);}
        return result;
    }

    size_t edgeCount() const override { return edgeMap.size();}

    void forEachEdge(const typename Graph<V,E>::EdgeVisitor&) const override;
    
    // utilities

    /**
     * public method for returning a vector of pointers to vertices adjacent to a given vertex
     * @param vID ID of vertex to find neighbors of
     * @return vector of pointers to neighboring vertices
     * iterates through the incident edges of the vertex and uses @fn opposite() to find the neighboring vertex
     */
    std::vector<VertexPtr> neighbors(VertexID vID) const override {
        std::vector<VertexPtr> result;
        auto vertex = this->getVertex(vID);
        const auto& edgeList = directed ? vertex->outEdges()
                                        : vertex->incidentEdges();
        for (const auto& edge : edgeList)
            if (auto neigh = edge->opposite(vID)) { result.push_back(this->getVertex(*neigh)); }
        return result;
    }

    void print() const override;

    size_t degree(VertexID) const override;

    bool isDirected() const override { return directed;}

private:
    std::unordered_map<VertexID, VertexPtr> vertexMap;          // mapping VertexID to Vertex object pointers
    std::unordered_map<EdgeID,   EdgePtr>   edgeMap;            // mapping EdgeID to Edge object pointers
    bool directed;                                             // flag for directed or undirected graph
    VertexID nextVID = 0;
    EdgeID   nextEID = 0;

    /**
     * private method for erasing and edge by its ID
     * @param eID ID of edge to be erased
     * checks if edge with the given ID exists; if not, returns. Otherwise, proceeds
     * gets the edge object from map using key passed as parameter, instead of iterating through the list of edges
     * removes the edge from the incident edge lists of its source and destination vertices
     * erases the edge from the edge map
     */
    void eraseEdgeByID(EdgeID eID) {
        auto it = edgeMap.find(eID);

        if(it == edgeMap.end()) { return;}
        auto& edge = it->second;

        if(this->hasVertex(edge->getFrom())) { this->getVertex(edge->getFrom())->removeEdge(eID);}
        if(this->hasVertex(edge->getTo()))  { this->getVertex(edge->getTo())->removeEdge(eID);}

        edgeMap.erase(eID);
    }
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
void AdjListGraph<V,E>::eraseVertex(VertexID old) {
    auto vertex = this->getVertex(old);

    std::vector<EdgeID> toBeRemoved;

    for (const auto& edge : vertex->incidentEdges()) { toBeRemoved.push_back(edge->getID());}
    for (EdgeID eID : toBeRemoved) { eraseEdgeByID(eID);}
    vertexMap.erase(old);
}

/**
 * public method for erasing an edge from the graph
 * @tparam V type of name of the vertex, @tparam E type of weight
 * @param from source vertex ID, @param to destination vertex ID
 * @throws EdgeNotFoundException if there is no edge between the given vertices
 * gets the edge object from map using keys passed as parameters
 */
template <typename V, typename E>
void AdjListGraph<V,E>::eraseEdge(VertexID from, VertexID to) {
    auto edge = this->findEdge(from, to);
    if (!edge) {throw EdgeNotFoundException(from, to);}
    eraseEdgeByID((*edge)->getID());
}

/**
 * public method for printing the graph
 * @tparam V type of name of the vertex, @tparam E type of weight
 * iterates through the vertex map, ignoring the keys and prints the name of the vertex and the names of its neighboring vertices
 */
template <typename V, typename E>
void AdjListGraph<V,E>::print() const {
    for (const auto& [_, vertex] : vertexMap) {
            std::cout << vertex->getName() << ": ";
            for (const auto& neigh : neighbors(vertex->getID()))
                std::cout << neigh->getName() << " ";
            std::cout << "\n";
    }
}

/**
 * public method for returning the degree of a vertex
 * @tparam V type of name of the vertex, @tparam E type of weight
 * @param vID ID of the vertex to return the degree of
 * @return degree of vertex with the given ID
 * gets the vertex object from map using key passed as parameter and calls the @fn degree() of vertex object
 */
template <typename V, typename E>
size_t AdjListGraph<V,E>::degree(VertexID vID) const { return this->getVertex(vID)->degree();}

template <typename V, typename E>
void AdjListGraph<V,E>::forEachEdge(const typename Graph<V,E>::EdgeVisitor& fn) const {
    for (const auto& [_, edge] : edgeMap) { fn(edge->getFrom(), edge->getTo(), edge->getWeight());}
}