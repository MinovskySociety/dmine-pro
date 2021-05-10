#ifndef _GUNDAM_TOOL_CONNECTED_H
#define _GUNDAM_TOOL_CONNECTED_H

#include "gundam/algorithm/dfs.h"

#include "gundam/type_getter/vertex_handle.h"

namespace GUNDAM {

template <typename GraphType>
inline bool Connected(GraphType& graph) {
  // just begin bfs at a random vertex and find whether
  // it can reach all vertexes
  typename VertexHandle<GraphType>::type
           vertex_handle = graph.VertexBegin();
  return GUNDAM::Dfs<true>(graph,vertex_handle)
                        == graph.CountVertex();
}

}  // namespace GUNDAM

#endif // _GUNDAM_TOOL_CONNECTED_H