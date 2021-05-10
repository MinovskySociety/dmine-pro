#ifndef _GUNDAM_ALGORITHM_DFS_H
#define _GUNDAM_ALGORITHM_DFS_H

#include <stack>
#include <set>
#include <type_traits>

#include "gundam/type_getter/vertex_handle.h"
#include "gundam/type_getter/edge_handle.h"

namespace GUNDAM {

// legal prune_callback forms:
//    prune_callback(vertex_handle)
//    prune_callback(vertex_handle, edge_handle)
//    prune_callback(vertex_handle, edge_handle, dfs_idx)
//    prune_callback(vertex_handle, edge_handle, dfs_idx, dfs_depth)
//
// legal user_callback forms:
//    user_callback(vertex_handle)
//    user_callback(vertex_handle, dfs_idx)
//    user_callback(vertex_handle, dfs_idx, dfs_depth)
template <bool bidirectional = false,
          bool remove_duplicate = true,
          typename         GraphType,
          typename  UserCallBackType,
          typename PruneCallBackType,
          typename std::enable_if_t< 
                   // prune_callback(next_vertex_handle)
                   std::is_convertible_v<
                            PruneCallBackType, 
                            std::function<bool(typename VertexHandle<GraphType>::type)> >
                || // prune_callback(next_vertex_handle, next_edge_handle)
                   std::is_convertible_v<
                            PruneCallBackType, 
                            std::function<bool(typename VertexHandle<GraphType>::type, 
                                               typename   EdgeHandle<GraphType>::type)> >
                || // prune_callback(next_vertex_handle, next_edge_handle, current_dfs_idx)
                   std::is_convertible_v<
                            PruneCallBackType, 
                            std::function<bool(typename VertexHandle<GraphType>::type, 
                                               typename   EdgeHandle<GraphType>::type,
                                               typename  GraphType::VertexCounterType)> >
                || // prune_callback(next_vertex_handle, next_edge_handle, current_dfs_idx, current_distance)
                   std::is_convertible_v<
                            PruneCallBackType, 
                            std::function<bool(typename VertexHandle<GraphType>::type, 
                                               typename   EdgeHandle<GraphType>::type,
                                               typename  GraphType::VertexCounterType,
                                               typename  GraphType::VertexCounterType)> >, bool> = false>
inline size_t Dfs(GraphType& graph,
              typename VertexHandle<GraphType>::type& src_vertex_handle,
           UserCallBackType  user_callback,
          PruneCallBackType prune_callback) {
  using VertexCounterType = typename  GraphType::VertexCounterType;
  using  VertexHandleType = typename VertexHandle<GraphType>::type;
  using    EdgeHandleType = typename   EdgeHandle<GraphType>::type;
  static_assert(
       // user_callback(vertex_handle)
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType)> >
    || // user_callback(vertex_handle, dfs_idx)
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType, 
                                   VertexCounterType)> >
    || // user_callback(vertex_handle, dfs_idx, dfs_depth)
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType, 
                                   VertexCounterType, 
                                   VertexCounterType)> >,
      "illegal callback type, only allows one of user_callback(vertex_handle) and user_callback(vertex_handle, dfs_idx)");

  static_assert(
       // prune_callback(vertex_handle)
      std::is_convertible_v<
                PruneCallBackType, 
                std::function<bool(VertexHandleType)> >
    || // prune_callback(vertex_handle, edge_handle)
      std::is_convertible_v<
                PruneCallBackType, 
                std::function<bool(VertexHandleType, 
                                     EdgeHandleType)> >
    || // prune_callback(vertex_handle, edge_handle, dfs_idx)
      std::is_convertible_v<
                PruneCallBackType, 
                std::function<bool(VertexHandleType, 
                                     EdgeHandleType,
                                  VertexCounterType)> >
    || // prune_callback(vertex_handle, edge_handle, dfs_idx, dfs_depth)
      std::is_convertible_v<
                PruneCallBackType, 
                std::function<bool(VertexHandleType, 
                                     EdgeHandleType,
                                  VertexCounterType,
                                  VertexCounterType)> >,
      "illegal callback type, only allows one of prune_callback(vertex_handle), prune_callback(vertex_handle, edge_handle) and prune_callback(vertex_handle, edge_handle, dfs_idx)");

  VertexCounterType dfs_idx = 0;
  std::stack<std::pair<VertexHandleType, 
                       VertexCounterType>> vertex_handle_stack;
  std:: set <VertexHandleType> visited;
  vertex_handle_stack.emplace(src_vertex_handle, 0);
  if constexpr (remove_duplicate){
    visited.emplace(src_vertex_handle);
  }
  while (!vertex_handle_stack.empty()) {
    auto [current_vertex_handle,
          current_dfs_depth] = vertex_handle_stack.top();
    vertex_handle_stack.pop();
    bool ret = false;
    if constexpr (
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType)> >){
      ret = user_callback(current_vertex_handle);
    }
    if constexpr (
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType,
                                  VertexCounterType)> >){
      ret = user_callback(current_vertex_handle,
                          dfs_idx);
    }
    if constexpr (
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType,
                                  VertexCounterType,
                                  VertexCounterType)> >){
      ret = user_callback(current_vertex_handle,
                          dfs_idx,
                          current_dfs_depth);
    }
    dfs_idx++;
    if (!ret){
      // meets stopping condition, stop the matching process
      return dfs_idx;
    }
    for (auto edge_it = current_vertex_handle->OutEdgeBegin();
             !edge_it.IsDone();
              edge_it++) {
      if constexpr (remove_duplicate){
        if (visited.find(edge_it->dst_handle()) != visited.end()){
          // already visited
          continue;
        }
      }
      bool prune_ret = false;
      if constexpr (
        // prune_callback(vertex_handle)
        std::is_convertible_v<
                  PruneCallBackType, 
                  std::function<bool(VertexHandleType)> >){
        prune_ret = prune_callback(edge_it->dst_handle());
      }
      if constexpr (
        // prune_callback(vertex_handle, edge_handle)
        std::is_convertible_v<
                  PruneCallBackType, 
                  std::function<bool(VertexHandleType, 
                                       EdgeHandleType)> >){
        prune_ret = prune_callback(edge_it->dst_handle(),
                                   edge_it);
      }
      if constexpr (
        // prune_callback(vertex_handle, edge_handle, dfs_idx)
        std::is_convertible_v<
                  PruneCallBackType, 
                  std::function<bool(VertexHandleType, 
                                       EdgeHandleType,
                                    VertexCounterType)> >){
        prune_ret = prune_callback(edge_it->dst_handle(),
                                   edge_it, dfs_idx);
      }
      if constexpr (
        // prune_callback(vertex_handle, edge_handle, dfs_idx, dfs_depth)
        std::is_convertible_v<
                  PruneCallBackType, 
                  std::function<bool(VertexHandleType, 
                                       EdgeHandleType,
                                    VertexCounterType,
                                    VertexCounterType)> >){
        prune_ret = prune_callback(edge_it->dst_handle(),
                                   edge_it, dfs_idx,
                                   current_dfs_depth + 1);
      }
      if (prune_ret){
        // this vertex is pruned, does not be considered
        continue;
      }
      if constexpr (remove_duplicate){
        visited.emplace(edge_it->dst_handle());
      }
      vertex_handle_stack.emplace(edge_it->dst_handle(),
                                  current_dfs_depth + 1);
    }
    if constexpr (bidirectional){
      for (auto edge_it = current_vertex_handle->InEdgeBegin();
               !edge_it.IsDone();
                edge_it++) {
        if constexpr (remove_duplicate){
          if (visited.find(edge_it->src_handle()) != visited.end()){
            // already visited
            continue;
          }
        }
        bool prune_ret = false;
        if constexpr (
          std::is_convertible_v<
                    PruneCallBackType, 
                    std::function<bool(VertexHandleType)> >){
          prune_ret = prune_callback(edge_it->src_handle());
        }
        if constexpr (
          std::is_convertible_v<
                    PruneCallBackType, 
                    std::function<bool(VertexHandleType, 
                                        EdgeHandleType)> >){
          prune_ret = prune_callback(edge_it->src_handle(),
                                     edge_it);
        }
        if constexpr (
          std::is_convertible_v<
                    PruneCallBackType, 
                    std::function<bool(VertexHandleType, 
                                         EdgeHandleType,
                                      VertexCounterType)> >){
          prune_ret = prune_callback(edge_it->src_handle(),
                                     edge_it, dfs_idx);
        }
        if constexpr (
          std::is_convertible_v<
                    PruneCallBackType, 
                    std::function<bool(VertexHandleType, 
                                         EdgeHandleType,
                                      VertexCounterType,
                                      VertexCounterType)> >){
          prune_ret = prune_callback(edge_it->src_handle(),
                                     edge_it, dfs_idx,
                                     current_dfs_depth + 1);
        }
        if (prune_ret){
          // this vertex is pruned, does not be considered
          continue;
        }
        if constexpr (remove_duplicate){
          visited.emplace(edge_it->src_handle());
        }
        vertex_handle_stack.emplace(edge_it->src_handle(),
                                    current_dfs_depth + 1);
      }
    }
  }
  return dfs_idx;
}

template <bool bidirectional = false,
          bool remove_duplicate = true,
          typename GraphType>
inline size_t Dfs(GraphType& graph,
              typename VertexHandle<GraphType>::type& src_vertex_handle) {
  using VertexHandleType = typename VertexHandle<GraphType>::type;
  auto do_nothing_callback = [](const VertexHandleType& vertex_handle, 
                                const size_t&           dfs_idx){
    // do nothing, continue matching
    return true;
  };
  return Dfs<bidirectional,
             remove_duplicate>(graph, src_vertex_handle, do_nothing_callback);
}

template <bool bidirectional = false,
          bool remove_duplicate = true,
          typename        GraphType,
          typename UserCallBackType>
inline size_t Dfs(GraphType&  graph,
  typename VertexHandle<GraphType>::type src_vertex_handle,
           UserCallBackType  user_callback,
           int distance_limit = -1) {
  using VertexCounterType = typename  GraphType::VertexCounterType;
  using  VertexHandleType = typename VertexHandle<GraphType>::type;
  using    EdgeHandleType = typename   EdgeHandle<GraphType>::type;
  auto prune_distance_callback = [&distance_limit](
         VertexHandleType  vertex_handle,
           EdgeHandleType    edge_handle,
        VertexCounterType current_idx,
        VertexCounterType current_distance){
    if (distance_limit == -1){
      // distance limit is not set, prune nothing
      return false;
    }
    assert(current_distance <= distance_limit + 1);
    if (current_distance > distance_limit) {
      // reach the distance limit, prune this vertex
      return true;
    }
    // does not reach the limit, do not prune
    return false;
  };
  return Dfs<bidirectional,
             remove_duplicate>(graph,
                   src_vertex_handle, 
                       user_callback, 
             prune_distance_callback); 
}

}  // namespace GUNDAM

#endif // _DFS_H
