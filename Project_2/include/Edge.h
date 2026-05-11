#pragma once
#include <optional>
#include "Utilities.h"

/**
 * class Edge
 * represents a connection between two vertices in a graph
 * @tparam E the type of the weight of the edege such as int, float etc.
 */
template <typename E>
class Edge {
private:
    EdgeID      id;
    VertexID    from;
    VertexID    to;
    E           weight;
    
public:
    Edge();
    Edge(EdgeID, VertexID, VertexID, E);
    ~Edge();

    std::optional<VertexID> opposite(VertexID) const;
    bool isAdjacentTo(const Edge<E>&) const;
    bool isIncidentOn(VertexID) const;

    E getWeight() const {return weight;}
    void setWeight(E newW) {weight = newW;}

    EdgeID getID() const {return id;}
    void setID(EdgeID newID) {id = newID;}

    VertexID getFrom() const {return from;}
    VertexID getTo() const {return to;}

    bool operator==(const Edge<E>& e) {return id == e.getID();}

};

/**
 * non-parameter constructor of @class Edge
 * @tparam E the type of the weight of the edge
 * sets all edge attributes to 0, except weight which is set to 1
 */
template <typename E>
Edge<E>::Edge() : id{0}, from{0}, to{0}, weight{1} {}

/**
 * parameter constructor of @class Edge
 * @tparam E the type of the weight of the edge
 * @param newID the id of edge, @param newFrom source vertex ID, @param newTo destination vertex ID, @param newWeight weight of the edge
 * sets the edge attributes to the given parameters
 */
template <typename E>
Edge<E>::Edge(EdgeID newID, VertexID newFrom, VertexID newTo, E newWeight) : id{newID}, from{newFrom}, to{newTo}, weight{newWeight} {}

/**
 * destructor of @class Edge
 */
template <typename E>
Edge<E>::~Edge() {}

/**
 * public method for returning the opposite vertex ID selected vertex
 * @param oppositeV the selected vertex ID
 * @return the opposite vertex ID if available, otherwise nullopt
 */
template <typename E>
std::optional<VertexID> Edge<E>::opposite(VertexID oppositeV) const {
    if(from == oppositeV)   return to;
    if(to == oppositeV)     return from;
    return std::nullopt;
}

/**
 * public method for checking if the edge is adjacent to another edge
 * @param next the other object of @class Edge to be checked
 * @return true if two edges are adjacent, otherwise false
 */
template <typename E>
bool Edge<E>::isAdjacentTo(const Edge<E>& next) const {return next.isIncidentOn(from) || next.isIncidentOn(to);}

/**
 * public method for checking if the edge is incident to a vertex
 * @param neigh the vertex ID to be checked for
 * @return true if the edge is incident to the vertex, otherwise false
 */
template <typename E>
bool Edge<E>::isIncidentOn(VertexID neigh) const {return from == neigh || to == neigh;}