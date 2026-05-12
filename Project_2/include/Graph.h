#pragma once
#include "Edge.h"
#include "Vertex.h"
#include <functional>

/**
 * class Graph
 * implements an abstract graph data structure that is later overridden by implementations
 * @tparam V type of name of the vertex, @tparam E type of weight of the edge
 */
template <typename V, typename E>
class Graph {
public:

    using VertexPtr     = std::shared_ptr<Vertex<V,E>>;
    using EdgePtr       = std::shared_ptr<Edge<E>>;
    using EdgeVisitor   = std::function<void(VertexID, VertexID, E)>;
    
    Graph()                                                                   = default;
    virtual ~Graph()                                                          = default;

    virtual VertexPtr               addVertex(V)                              = 0;
    virtual void                    eraseVertex(VertexID)                     = 0;
    virtual VertexPtr               getVertex(VertexID)                 const = 0;
    virtual bool                    hasVertex(VertexID)                 const = 0;
    virtual std::vector<VertexPtr>  vertices()                          const = 0;
    virtual size_t                  vertexCount()                       const = 0;

    virtual EdgePtr                 addEdge(VertexID, VertexID, E)            = 0;
    virtual void                    eraseEdge(VertexID,VertexID)              = 0;
    virtual bool                    hasEdge(VertexID, VertexID)         const = 0;
    virtual std::optional<EdgePtr>  findEdge(VertexID, VertexID)        const = 0;
    virtual std::vector<EdgePtr>    edges()                             const = 0;

    virtual void                    forEachEdge(const EdgeVisitor&)     const = 0;

    virtual size_t                  edgeCount()                         const = 0;

    virtual std::vector<VertexPtr>  neighbors(VertexID)                 const = 0;
    virtual void                    print()                             const = 0;

    virtual size_t                  degree(VertexID)                    const = 0;


};