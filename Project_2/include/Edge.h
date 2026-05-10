#pragma once
#include <optional>
#include "Utilities.h"

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

template <typename E>
Edge<E>::Edge() : id{0}, from{0}, to{0}, weight{1} {}

template <typename E>
Edge<E>::Edge(EdgeID newID, VertexID newFrom, VertexID newTo, E newWeight) : id{newID}, from{newFrom}, to{newTo}, weight{newWeight} {}

template <typename E>
Edge<E>::~Edge() {}

template <typename E>
std::optional<VertexID> Edge<E>::opposite(VertexID oppositeV) const {
    if(from == oppositeV)   return to;
    if(to == oppositeV)     return from;
    return std::nullopt;
}

template <typename E>
bool Edge<E>::isAdjacentTo(const Edge<E>& next) const {return next.isIncidentOn(from) || next.isIncidentOn(to);}

template <typename E>
bool Edge<E>::isIncidentOn(VertexID neigh) const {return from == neigh || to == neigh;}