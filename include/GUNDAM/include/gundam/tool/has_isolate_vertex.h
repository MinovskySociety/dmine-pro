#ifndef _GUNDAM_TOOL_HAS_ISOLATE_VERTEX_H
#define _GUNDAM_TOOL_HAS_ISOLATE_VERTEX_H

namespace GUNDAM {

template <typename GraphType>
inline bool HasIsolateVertex(GraphType& graph) {
  // just begin bfs at a random vertex and find whether
  // it can reach all vertexes
  for (auto vertex_it = graph.VertexBegin();
           !vertex_it.IsDone();
            vertex_it++){
    if (vertex_it->CountOutEdge() == 0
     && vertex_it->CountInEdge() == 0){
      // this vertex is isolated
      return true;
    }
  }
  return false;
}

}  // namespace GUNDAM

#endif // _GUNDAM_TOOL_HAS_ISOLATE_VERTEX_H