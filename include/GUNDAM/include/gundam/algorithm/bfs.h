#ifndef _BFS_H
#define _BFS_H

#include <queue>
#include <set>

namespace GUNDAM {

template <bool bidirectional = false,
          typename        GraphType,
          typename    VertexPtrType,
          typename UserCallBackType>
inline size_t Bfs(GraphType&  graph,
     std::set<VertexPtrType>& src_vertex_ptr_set,
           UserCallBackType&  user_callback) {
  size_t counter = 0;
  std::queue<std::pair<VertexPtrType, size_t>> vertex_ptr_queue;
  std:: set <VertexPtrType> visited;
  for (const auto& src_vertex_ptr : src_vertex_ptr_set){
    vertex_ptr_queue.emplace(src_vertex_ptr, 0);
    visited.emplace(src_vertex_ptr);
  }
  while (!vertex_ptr_queue.empty()){
    auto [current_vertex_ptr, 
          current_distance] = vertex_ptr_queue.front();
    vertex_ptr_queue.pop();
    counter++;
    if (!user_callback(current_vertex_ptr, current_distance)){
      // meets stopping condition, stop the matching process
      return counter;
    }
    for (auto edge_it = current_vertex_ptr->OutEdgeBegin();
             !edge_it.IsDone();
              edge_it++) {
      if (visited.find(edge_it->dst_ptr()) != visited.end()){
        // already visited
        continue;
      }
      visited.emplace(edge_it->dst_ptr());
      vertex_ptr_queue.emplace(edge_it->dst_ptr(), current_distance + 1);
    }
    if constexpr (bidirectional){
      for (auto edge_it = current_vertex_ptr->InEdgeBegin();
               !edge_it.IsDone();
                edge_it++) {
        if (visited.find(edge_it->src_ptr()) != visited.end()){
          // already visited
          continue;
        }
        visited.emplace(edge_it->src_ptr());
        vertex_ptr_queue.emplace(edge_it->src_ptr(), current_distance + 1);
      }
    }
  }
  return counter;
}

template <bool bidirectional = false,
          typename        GraphType,
          typename    VertexPtrType,
          typename UserCallBackType>
inline size_t Bfs(GraphType& graph,
              VertexPtrType& src_vertex_ptr,
           UserCallBackType& user_callback) {
  std::set<VertexPtrType> src_vertex_ptr_set = {src_vertex_ptr};
  return Bfs<bidirectional>(graph, src_vertex_ptr_set, user_callback);
}

template <bool bidirectional = false,
          typename        GraphType,
          typename    VertexPtrType>
inline size_t Bfs(GraphType& graph,
              VertexPtrType& src_vertex_ptr) {
  std::set<VertexPtrType> src_vertex_ptr_set = {src_vertex_ptr};

  auto do_nothing_callback = [](const VertexPtrType& vertex_ptr, 
                                const size_t& current_distance){
    // do nothing, continue matching
    return true;
  };
  return Bfs<bidirectional>(graph, src_vertex_ptr_set, do_nothing_callback);
}

}  // namespace GUNDAM

#endif // _BFS_H
