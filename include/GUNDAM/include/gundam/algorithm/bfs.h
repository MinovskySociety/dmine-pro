#ifndef _BFS_H
#define _BFS_H

#include <queue>
#include <set>
#include <type_traits>

namespace GUNDAM {

// legal callback forms:
//    user_callback(vertex_ptr)
//    user_callback(vertex_ptr, bfs_idx)
//    user_callback(vertex_ptr, bfs_idx, distance)
template <bool bidirectional = false,
          typename        GraphType,
          typename    VertexPtrType,
          typename UserCallBackType>
inline size_t Bfs(GraphType&  graph,
     std::set<VertexPtrType>& src_vertex_ptr_set,
           UserCallBackType  user_callback) {
  using VertexCounterType = typename GraphType::VertexCounterType;
  static_assert(
       // user_callback(vertex_ptr)
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexPtrType)> >
    || // user_callback(vertex_ptr, bfs_idx)
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexPtrType, 
                                   VertexCounterType)> >
    || // user_callback(vertex_ptr, bfs_idx, distance)
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexPtrType, 
                                   VertexCounterType, 
                                   VertexCounterType)> >, 
      "illegal callback type, only allows one of user_callback(vertex_ptr), user_callback(vertex_ptr, bfs_idx) and user_callback(vertex_ptr, bfs_idx, distance)");
  VertexCounterType bfs_idx = 0;
  std::queue<std::pair<VertexPtrType, VertexCounterType>> vertex_ptr_queue;
  std:: set <VertexPtrType> visited;
  for (const auto& src_vertex_ptr : src_vertex_ptr_set){
    vertex_ptr_queue.emplace(src_vertex_ptr, 0);
    visited.emplace(src_vertex_ptr);
  }
  while (!vertex_ptr_queue.empty()){
    auto [current_vertex_ptr, 
          current_distance] = vertex_ptr_queue.front();
    vertex_ptr_queue.pop();
    bfs_idx++;
    bool ret = false;
    if constexpr (
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexPtrType)> >){
      ret = user_callback(current_vertex_ptr);
    }
    if constexpr (
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexPtrType,
                                   VertexCounterType)> >){
      ret = user_callback(current_vertex_ptr,
                          bfs_idx);
    }
    if constexpr (
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexPtrType,
                                   VertexCounterType,
                                   VertexCounterType)> >){
      ret = user_callback(current_vertex_ptr,
                          bfs_idx,
                          current_distance);
    }
    if (!ret){
      // meets stopping condition, stop the matching process
      return bfs_idx;
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
  return bfs_idx;
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
          typename     GraphType,
          typename VertexPtrType>
inline size_t Bfs(GraphType& graph,
              VertexPtrType& src_vertex_ptr) {

  auto do_nothing_callback = [](VertexPtrType vertex_ptr, 
                                typename GraphType::VertexCounterType current_distance){
    // do nothing, continue matching
    return true;
  };
  return Bfs<bidirectional>(graph, src_vertex_ptr, do_nothing_callback);
}

}  // namespace GUNDAM

#endif // _BFS_H
