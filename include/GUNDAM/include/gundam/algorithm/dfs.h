#ifndef _DFS_H
#define _DFS_H

#include <stack>
#include <set>
#include <type_traits>

namespace GUNDAM {

// legal callback forms:
//    user_callback(vertex_ptr)
//    user_callback(vertex_ptr, dfs_idx)
template <bool bidirectional = false,
          typename        GraphType,
          typename    VertexPtrType,
          typename UserCallBackType>
inline size_t Dfs(GraphType& graph,
              VertexPtrType& src_vertex_ptr,
           UserCallBackType& user_callback) {
  using VertexCounterType = typename GraphType::VertexCounterType;
  static_assert(
       // user_callback(vertex_ptr)
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexPtrType)> >
    || // user_callback(vertex_ptr, dfs_idx)
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexPtrType, 
                                   VertexCounterType)> >,
      "illegal callback type, only allows one of user_callback(vertex_ptr) and user_callback(vertex_ptr, bfs_idx)");

  VertexCounterType dfs_idx = 0;
  std::stack<VertexPtrType> vertex_ptr_stack;
  std:: set <VertexPtrType> visited;
  vertex_ptr_stack.emplace(src_vertex_ptr);
  visited.emplace(src_vertex_ptr);
  while (!vertex_ptr_stack.empty()) {
    auto current_vertex_ptr = vertex_ptr_stack.top();
    vertex_ptr_stack.pop();
    dfs_idx++;
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
                          dfs_idx);
    }
    if (!ret){
      // meets stopping condition, stop the matching process
      return dfs_idx;
    }
    for (auto edge_it = current_vertex_ptr->OutEdgeBegin();
             !edge_it.IsDone();
              edge_it++) {
      if (visited.find(edge_it->dst_ptr()) != visited.end()){
        // already visited
        continue;
      }
      visited.emplace(edge_it->dst_ptr());
      vertex_ptr_stack.emplace(edge_it->dst_ptr());
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
        vertex_ptr_stack.emplace(edge_it->src_ptr());
      }
    }
  }
  return dfs_idx;
}

template <bool bidirectional = false,
          typename     GraphType,
          typename VertexPtrType>
inline size_t Dfs(GraphType& graph,
              VertexPtrType& src_vertex_ptr) {

  auto do_nothing_callback = [](const VertexPtrType& vertex_ptr, 
                                const size_t&           dfs_idx){
    // do nothing, continue matching
    return true;
  };
  return Dfs<bidirectional>(graph, src_vertex_ptr, do_nothing_callback);
}

}  // namespace GUNDAM

#endif // _DFS_H
