#ifndef _DFS_H
#define _DFS_H

#include <stack>
#include <set>

namespace GUNDAM {

template <bool bidirectional = false,
          typename        GraphType,
          typename    VertexPtrType,
          typename UserCallBackType>
inline size_t Dfs(GraphType& graph,
              VertexPtrType& src_vertex_ptr,
           UserCallBackType& user_callback) {

  size_t counter = 0;
  std::stack<VertexPtrType> vertex_ptr_stack;
  std:: set <VertexPtrType> visited;
  vertex_ptr_stack.emplace(src_vertex_ptr);
  visited.emplace(src_vertex_ptr);
  while (!vertex_ptr_stack.empty()) {
    auto current_vertex_ptr = vertex_ptr_stack.top();
    vertex_ptr_stack.pop();
    if (!user_callback(current_vertex_ptr, 
                       counter)){
      // meets stopping condition, stop the matching process
      return counter;
    }
    counter++;
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
  return counter;
}

template <bool bidirectional = false,
          typename        GraphType,
          typename    VertexPtrType>
inline size_t Dfs(GraphType& graph,
              VertexPtrType& src_vertex_ptr) {

  auto do_nothing_callback = [](const VertexPtrType& vertex_ptr, 
                                const size_t&        vertex_idx){
    // do nothing, continue matching
    return true;
  };
  return Dfs<bidirectional>(graph, src_vertex_ptr, do_nothing_callback);
}

}  // namespace GUNDAM

#endif // _DFS_H
