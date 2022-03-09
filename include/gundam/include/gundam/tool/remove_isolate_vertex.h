#ifndef _GUNDAM_TOOL_REMOVE_ISOLATE_VERTEX_H
#define _GUNDAM_TOOL_REMOVE_ISOLATE_VERTEX_H

#include <set>

namespace GUNDAM {

// remove all isolated vertexes and 
// return how many vertexes are removed
template <typename GraphType>
inline size_t RemoveIsolateVertex( GraphType& graph) {
  static_assert(!std::is_const_v<GraphType>, "graph should not be constant");
  using VertexIDType = typename GraphType::VertexType::IDType;
  std::set<VertexIDType> isolated_vertex_id_set;
  // just begin bfs at a random vertex and find whether
  // it can reach all vertexes
  for (auto vertex_it = graph.VertexBegin();
           !vertex_it.IsDone();
            vertex_it++){
    if (vertex_it->CountOutEdge() != 0
     || vertex_it->CountInEdge() != 0){
      // this vertex is not isolated
      continue;
    }
    // this vertex is isolated
    auto [ret_it, ret] = isolated_vertex_id_set.emplace(vertex_it->id());
    assert(ret);
  }
  for (const auto& isolated_vertex_id
                 : isolated_vertex_id_set){
    graph.EraseVertex(isolated_vertex_id);
  }
  return isolated_vertex_id_set.size();
}

}  // namespace GUNDAM

#endif // _GUNDAM_TOOL_REMOVE_ISOLATE_VERTEX_H