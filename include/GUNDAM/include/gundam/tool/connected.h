#ifndef _CONNECTED_H
#define _CONNECTED_H

#include "gundam/algorithm/dfs.h"

namespace GUNDAM {

template <typename GraphType>
inline bool Connected(GraphType& graph) {
  // just begin bfs at a random vertex and find whether
  // it can reach all vertexes
  const auto& graph_const_ref = graph;
  typename GraphType::VertexConstPtr 
    vertex_ptr = graph_const_ref.VertexBegin();
  return GUNDAM::Dfs<true>(graph_const_ref, vertex_ptr)
                        == graph_const_ref.CountVertex();
}

}  // namespace GUNDAM

#endif // _CONNECTED_H