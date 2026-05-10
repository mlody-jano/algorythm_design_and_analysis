#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

using VertexID  = std::size_t;
using EdgeID    = std::size_t;

struct GraphException : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct VertexNotFoundException : GraphException {
    explicit VertexNotFoundException(VertexID id) : GraphException("Vertex not found: " + std::to_string(id)) {}
};

struct EdgeNotFoundException : GraphException {
    explicit EdgeNotFoundException(VertexID from, VertexID to) : GraphException("Edge not found: " + std::to_string(from) + "-->" + std::to_string(to)) {} 
};

struct DuplicateEdgeException : GraphException {
    explicit DuplicateEdgeException(VertexID from, VertexID to) : GraphException("Edge already exists: " + std::to_string(from) + "-->" + std::to_string(to)) {}
};