#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * public types for identifying vertices of @class Vertex in class @class Graph
 */
using VertexID  = std::size_t;

/**
 * public types for identifying edges of @class Edge in class @class Graph
 */
using EdgeID    = std::size_t;
/**
 * structure GraphException
 * base class for all exception thrown by @class Graph
 * derevied class of std::runtime_error
 * used to handle exception in @class Graph, such as vertex not found, edge not found, duplicate edge, etc.
 */
struct GraphException : std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
 * structure VertexNotFoundException
 * derived class of GraphException
 * @param id the id of the vertex that is not found
 * @throws GraphException when the vertex is not found in the graph
 */
struct VertexNotFoundException : GraphException {
    explicit VertexNotFoundException(VertexID id) : GraphException("Vertex not found: " + std::to_string(id)) {}
};

/**
 * structure EdgeNotFoundException
 * derived class of GraphException
 * @param from refers to the source vertex ID, @param to refers to the destination vertex ID
 * @throws GraphException when the edge is not found in the graph
 */
struct EdgeNotFoundException : GraphException {
    explicit EdgeNotFoundException(VertexID from, VertexID to) : GraphException("Edge not found: " + std::to_string(from) + "-->" + std::to_string(to)) {} 
};
/**
 * structure DuplicateEdgeException
 * derived class of GraphException
 * @param from refers to the source vertex ID, @param to refers to the destination vertex
 * @throws GraphException when the edge alreasy exists in the graph
 */
struct DuplicateEdgeException : GraphException {
    explicit DuplicateEdgeException(VertexID from, VertexID to) : GraphException("Edge already exists: " + std::to_string(from) + "-->" + std::to_string(to)) {}
};